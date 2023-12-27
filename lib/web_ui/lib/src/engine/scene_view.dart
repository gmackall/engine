// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:async';

import 'package:ui/src/engine.dart';
import 'package:ui/ui.dart' as ui;

const String kCanvasContainerTag = 'flt-canvas-container';

// This is an interface that renders a `ScenePicture` as a `DomImageBitmap`.
// It is optionally asynchronous. It is required for the `EngineSceneView` to
// composite pictures into the canvases in the DOM tree it builds.
abstract class PictureRenderer {
  FutureOr<DomImageBitmap> renderPicture(ScenePicture picture);
}

class _SceneRender {
  _SceneRender(this.scene, this._completer) {
    scene.beginRender();
  }

  final EngineScene scene;
  final Completer<void> _completer;

  void done() {
    scene.endRender();
    _completer.complete();
  }
}

// This class builds a DOM tree that composites an `EngineScene`.
class EngineSceneView {
  factory EngineSceneView(PictureRenderer pictureRenderer) {
    final DomElement sceneElement = createDomElement('flt-scene');
    return EngineSceneView._(pictureRenderer, sceneElement);
  }

  EngineSceneView._(this.pictureRenderer, this.sceneElement);

  final PictureRenderer pictureRenderer;
  final DomElement sceneElement;

  List<SliceContainer> containers = <SliceContainer>[];

  _SceneRender? _currentRender;
  _SceneRender? _nextRender;

  Future<void> renderScene(EngineScene scene) {
    if (_currentRender != null) {
      // If a scene is already queued up, drop it and queue this one up instead
      // so that the scene view always displays the most recently requested scene.
      _nextRender?.done();
      final Completer<void> completer = Completer<void>();
      _nextRender = _SceneRender(scene, completer);
      return completer.future;
    }
    final Completer<void> completer = Completer<void>();
    _currentRender = _SceneRender(scene, completer);
    _kickRenderLoop();
    return completer.future;
  }

  Future<void> _kickRenderLoop() async {
    final _SceneRender current = _currentRender!;
    await _renderScene(current.scene);
    current.done();
    _currentRender = _nextRender;
    _nextRender = null;
    if (_currentRender == null) {
      return;
    } else {
      return _kickRenderLoop();
    }
  }

  Future<void> _renderScene(EngineScene scene) async {
    final List<LayerSlice> slices = scene.rootLayer.slices;
    final Iterable<Future<DomImageBitmap?>> renderFutures = slices.map(
      (LayerSlice slice) async => switch (slice) {
          PlatformViewSlice() => null,
          PictureSlice() => pictureRenderer.renderPicture(slice.picture),
        }
    );
    final List<DomImageBitmap?> renderedBitmaps = await Future.wait(renderFutures);
    final List<SliceContainer?> reusableContainers = List<SliceContainer?>.from(containers);
    final List<SliceContainer> newContainers = <SliceContainer>[];
    for (int i = 0; i < slices.length; i++) {
      final LayerSlice slice = slices[i];
      switch (slice) {
        case PictureSlice():
          PictureSliceContainer? container;
          for (int j = 0; j < reusableContainers.length; j++) {
            final SliceContainer? candidate = reusableContainers[j];
            if (candidate is PictureSliceContainer) {
              container = candidate;
              reusableContainers[j] = null;
              break;
            }
          }

          if (container != null) {
            container.bounds = slice.picture.cullRect;
          } else {
            container = PictureSliceContainer(slice.picture.cullRect);
          }
          container.updateContents();
          container.renderBitmap(renderedBitmaps[i]!);
          newContainers.add(container);

        case PlatformViewSlice():
          for (final PlatformView view in slice.views) {
            // TODO(harryterkelsen): Inject the FlutterView instance from `renderScene`,
            // instead of using `EnginePlatformDispatcher...implicitView` directly,
            // or make the FlutterView "register" like in canvaskit.
            // Ensure the platform view contents are injected in the DOM.
            EnginePlatformDispatcher.instance.implicitView?.dom.injectPlatformView(view.viewId);

            // Attempt to reuse a container for the existing view
            PlatformViewContainer? container;
            for (int j = 0; j < reusableContainers.length; j++) {
              final SliceContainer? candidate = reusableContainers[j];
              if (candidate is PlatformViewContainer && candidate.viewId == view.viewId) {
                container = candidate;
                reusableContainers[j] = null;
                break;
              }
            }
            container ??= PlatformViewContainer(view.viewId);
            container.size = view.size;
            container.styling = view.styling;
            container.updateContents();
            newContainers.add(container);
          }
      }
    }

    containers = newContainers;

    DomElement? currentElement = sceneElement.firstElementChild;
    for (final SliceContainer container in containers) {
      if (currentElement == null) {
        sceneElement.appendChild(container.container);
      } else if (currentElement == container.container) {
        currentElement = currentElement.nextElementSibling;
      } else {
        sceneElement.insertBefore(container.container, currentElement);
      }
    }

    // Remove any other unused containers
    while (currentElement != null) {
      final DomElement? sibling = currentElement.nextElementSibling;
      sceneElement.removeChild(currentElement);
      currentElement = sibling;
    }
  }
}

sealed class SliceContainer {
  DomElement get container;

  void updateContents();
}

final class PictureSliceContainer extends SliceContainer {
  factory PictureSliceContainer(ui.Rect bounds) {
    final DomElement container = domDocument.createElement(kCanvasContainerTag);
    final DomCanvasElement canvas = createDomCanvasElement(
      width: bounds.width.toInt(),
      height: bounds.height.toInt()
    );
    container.appendChild(canvas);
    return PictureSliceContainer._(bounds, container, canvas);
  }

  PictureSliceContainer._(this._bounds, this.container, this.canvas);

  ui.Rect _bounds;
  bool _dirty = true;

  ui.Rect get bounds => _bounds;
  set bounds(ui.Rect bounds) {
    if (_bounds != bounds) {
      _bounds = bounds;
      _dirty = true;
    }
  }

  @override
  void updateContents() {
    if (_dirty) {
      _dirty = false;

      final ui.Rect roundedOutBounds = ui.Rect.fromLTRB(
        bounds.left.floorToDouble(),
        bounds.top.floorToDouble(),
        bounds.right.ceilToDouble(),
        bounds.bottom.ceilToDouble()
      );
      final DomCSSStyleDeclaration style = canvas.style;
      final double devicePixelRatio = EngineFlutterDisplay.instance.devicePixelRatio;
      final double logicalWidth = roundedOutBounds.width / devicePixelRatio;
      final double logicalHeight = roundedOutBounds.height / devicePixelRatio;
      final double logicalLeft = roundedOutBounds.left / devicePixelRatio;
      final double logicalTop = roundedOutBounds.top / devicePixelRatio;
      style.width = '${logicalWidth}px';
      style.height = '${logicalHeight}px';
      style.position = 'absolute';
      style.left = '${logicalLeft}px';
      style.top = '${logicalTop}px';
      canvas.width = roundedOutBounds.width.ceilToDouble();
      canvas.height = roundedOutBounds.height.ceilToDouble();
    }
  }

  void renderBitmap(DomImageBitmap bitmap) {
    final DomCanvasRenderingContextBitmapRenderer ctx = canvas.contextBitmapRenderer;
    ctx.transferFromImageBitmap(bitmap);
  }

  @override
  final DomElement container;
  final DomCanvasElement canvas;
}

final class PlatformViewContainer extends SliceContainer {
  PlatformViewContainer(this.viewId) : container = createPlatformViewSlot(viewId);

  final int viewId;
  PlatformViewStyling? _styling;
  ui.Size? _size;
  bool _dirty = false;

  @override
  final DomElement container;

  set styling(PlatformViewStyling styling) {
    if (_styling != styling) {
      _styling = styling;
      _dirty = true;
    }
  }

  set size(ui.Size size) {
    if (_size != size) {
      _size = size;
      _dirty = true;
    }
  }


  @override
  void updateContents() {
    assert(_styling != null);
    assert(_size != null);
    if (_dirty) {
      final DomCSSStyleDeclaration style = container.style;
      final double devicePixelRatio = EngineFlutterDisplay.instance.devicePixelRatio;
      final double logicalWidth = _size!.width / devicePixelRatio;
      final double logicalHeight = _size!.height / devicePixelRatio;
      style.width = '${logicalWidth}px';
      style.height = '${logicalHeight}px';
      style.position = 'absolute';

      final ui.Offset? offset = _styling!.position.offset;
      final double logicalLeft = (offset?.dx ?? 0) / devicePixelRatio;
      final double logicalTop = (offset?.dy ?? 0) / devicePixelRatio;
      style.left = '${logicalLeft}px';
      style.top = '${logicalTop}px';

      final Matrix4? transform = _styling!.position.transform;
      style.transform = transform != null ? float64ListToCssTransform3d(transform.storage) : '';
      style.opacity = _styling!.opacity != 1.0 ? '${_styling!.opacity}' : '';
      // TODO(jacksongardner): Implement clip styling for platform views

      _dirty = false;
    }
  }
}
