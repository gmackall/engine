// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'package:ui/ui.dart' as ui;

import '../configuration.dart';
import '../dom.dart';
import '../safe_browser_api.dart';
import '../semantics/semantics.dart';
import 'style_manager.dart';

/// Manages DOM elements and the DOM structure for a [ui.FlutterView].
///
/// Here's the general DOM structure of a Flutter View:
///
/// [rootElement] <flutter-view>
///   |
///   +- [platformViewsHost] <flt-glass-pane>
///   |    |
///   |    +- [renderingHost] #shadow-root
///   |    |    |
///   |    |    +- <flt-semantics-placeholder>
///   |    |    |
///   |    |    +- [sceneHost] <flt-scene-host>
///   |    |    |    |
///   |    |    |    +- <flt-scene>
///   |    |    |
///   |    |    +- [announcementsHost] <flt-announcement-host>
///   |    |    |
///   |    |    +- <style>
///   |    |
///   |    +- ...platform views
///   |
///   +- [textEditingHost] <flt-text-editing-host>
///   |    |
///   |    +- ...text fields
///   |
///   +- [semanticsHost] <flt-semantics-host>
///   |    |
///   |    +- ...semantics nodes
///   |
///   +- <style>
///
class DomManager {
  factory DomManager({required double devicePixelRatio}) {
    final DomElement rootElement = domDocument.createElement(DomManager.flutterViewTagName);
    final DomElement platformViewsHost = domDocument.createElement(DomManager.glassPaneTagName);
    final DomShadowRoot renderingHost = _attachShadowRoot(platformViewsHost);
    final DomElement sceneHost = domDocument.createElement(DomManager.sceneHostTagName);
    final DomElement textEditingHost = domDocument.createElement(DomManager.textEditingHostTagName);
    final DomElement semanticsHost = domDocument.createElement(DomManager.semanticsHostTagName);
    final DomElement announcementsHost = createDomElement(DomManager.announcementsHostTagName);

    // Root element children.

    rootElement.appendChild(platformViewsHost);
    rootElement.appendChild(textEditingHost);
    // The semantic host goes last because hit-test order-wise it must be
    // first. If semantics goes under the scene host, platform views will
    // obscure semantic elements.
    //
    // You may be wondering: wouldn't semantics obscure platform views and
    // make then not accessible? At least with some careful planning, that
    // should not be the case. The semantics tree makes all of its non-leaf
    // elements transparent. This way, if a platform view appears among other
    // interactive Flutter widgets, as long as those widgets do not intersect
    // with the platform view, the platform view will be reachable.
    rootElement.appendChild(semanticsHost);

    // Rendering host (shadow root) children.

    // TODO(yjbanov): In a multi-view world, we may want to have a separate
    //                semantics owner for each view.
    //                https://github.com/flutter/flutter/issues/137445
    final DomElement accessibilityPlaceholder = EngineSemanticsOwner
        .instance.semanticsHelper
        .prepareAccessibilityPlaceholder();

    renderingHost.append(accessibilityPlaceholder);
    renderingHost.append(sceneHost);
    renderingHost.append(announcementsHost);

    // Styling.

    StyleManager.attachGlobalStyles(
      node: rootElement,
      styleId: 'flt-text-editing-stylesheet',
      styleNonce: configuration.nonce,
      cssSelectorPrefix: DomManager.flutterViewTagName,
    );

    StyleManager.attachGlobalStyles(
      node: renderingHost,
      styleId: 'flt-internals-stylesheet',
      styleNonce: configuration.nonce,
      cssSelectorPrefix: '',
    );

    StyleManager.styleSceneHost(
      sceneHost,
      debugShowSemanticsNodes: configuration.debugShowSemanticsNodes,
    );

    StyleManager.styleSemanticsHost(
      semanticsHost,
      devicePixelRatio,
    );

    return DomManager._(
      rootElement: rootElement,
      platformViewsHost: platformViewsHost,
      renderingHost: renderingHost,
      sceneHost: sceneHost,
      textEditingHost: textEditingHost,
      semanticsHost: semanticsHost,
      announcementsHost: announcementsHost,
    );
  }

  DomManager._({
    required this.rootElement,
    required this.platformViewsHost,
    required this.renderingHost,
    required this.sceneHost,
    required this.textEditingHost,
    required this.semanticsHost,
    required this.announcementsHost,
  });

  /// The tag name for the Flutter View root element.
  static const String flutterViewTagName = 'flutter-view';

  /// The tag name for the glass-pane.
  static const String glassPaneTagName = 'flt-glass-pane';

  /// The tag name for the scene host.
  static const String sceneHostTagName = 'flt-scene-host';

  /// The tag name for the text editing host.
  static const String textEditingHostTagName = 'flt-text-editing-host';

  /// The tag name for the semantics host.
  static const String semanticsHostTagName = 'flt-semantics-host';

  /// The tag name for the accessibility announcements host.
  static const String announcementsHostTagName = 'flt-announcement-host';

  /// The root DOM element for the entire Flutter View.
  ///
  /// This is where input events are captured, such as pointer events.
  ///
  /// If semantics is enabled, this element also contains the semantics DOM tree,
  /// which captures semantics input events.
  final DomElement rootElement;

  /// Hosts all platform view elements.
  final DomElement platformViewsHost;

  /// Hosts all rendering elements and canvases.
  final DomShadowRoot renderingHost;

  /// Hosts the <flt-scene> element.
  ///
  /// This element is created and inserted in the HTML DOM once. It is never
  /// removed or moved. However the <flt-scene> inside of it may be replaced.
  final DomElement sceneHost;

  /// Hosts all text editing elements.
  final DomElement textEditingHost;

  /// Hosts the semantics tree.
  ///
  /// This element is in front of the [renderingHost] and [platformViewsHost].
  /// Otherwise, the phone will disable focusing by touch, only by tabbing
  /// around the UI.
  final DomElement semanticsHost;

  /// This is where accessibility announcements are inserted.
  final DomElement announcementsHost;

  DomElement? _lastSceneElement;

  /// Inserts the [sceneElement] into the DOM and removes the existing scene (if
  /// any).
  ///
  /// The [sceneElement] is inserted  as a child of the <flt-scene-host> element
  /// inside the [renderingHost].
  ///
  /// If the [sceneElement] has already been inserted, this method does nothing
  /// to avoid unnecessary DOM mutations. This is both faster and more correct,
  /// because moving DOM nodes loses internal state, such as text selection.
  void setScene(DomElement sceneElement) {
    if (sceneElement != _lastSceneElement) {
      _lastSceneElement?.remove();
      _lastSceneElement = sceneElement;
      sceneHost.append(sceneElement);
    }
  }
}

DomShadowRoot _attachShadowRoot(DomElement element) {
  assert(
    getJsProperty<Object?>(element, 'attachShadow') != null,
    'ShadowDOM is not supported in this browser.',
  );

  return element.attachShadow(<String, dynamic>{
    'mode': 'open',
    // This needs to stay false to prevent issues like this:
    // - https://github.com/flutter/flutter/issues/85759
    'delegatesFocus': false,
  });
}
