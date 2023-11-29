// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:async';
import 'dart:js_util';

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';
import 'package:ui/src/engine.dart';

import '../../common/matchers.dart';

void main() {
  internalBootstrapBrowserTest(() => doTests);
}

Future<void> doTests() async {
  group('FlutterViewManager', () {
    final EnginePlatformDispatcher platformDispatcher = EnginePlatformDispatcher.instance;
    final FlutterViewManager viewManager = FlutterViewManager(platformDispatcher);

    group('registerView', () {
      test('can register view', () {
        final EngineFlutterView view = EngineFlutterView(platformDispatcher, createDomElement('div'));
        final int viewId = view.viewId;

        viewManager.registerView(view);

        expect(viewManager[viewId], view);
      });

      test('fails if the same viewId is already registered', () {
        final EngineFlutterView view = EngineFlutterView(platformDispatcher, createDomElement('div'));

        viewManager.registerView(view);

        expect(() { viewManager.registerView(view); }, throwsAssertionError);
      });

      test('stores JSOptions that getOptions can retrieve', () {
        final EngineFlutterView view = EngineFlutterView(platformDispatcher, createDomElement('div'));
        final int viewId = view.viewId;
        final JsFlutterViewOptions expectedOptions = jsify(<String, Object?>{
          'hostElement': createDomElement('div'),
        }) as JsFlutterViewOptions;

        viewManager.registerView(view, jsViewOptions: expectedOptions);

        final JsFlutterViewOptions? storedOptions = viewManager.getOptions(viewId);
        expect(storedOptions, expectedOptions);
      });
    });

    group('unregisterView', () {
      test('unregisters a view', () {
        final EngineFlutterView view = EngineFlutterView(platformDispatcher, createDomElement('div'));
        final int viewId = view.viewId;

        viewManager.registerView(view);
        expect(viewManager[viewId], isNotNull);

        viewManager.unregisterView(viewId);
        expect(viewManager[viewId], isNull);
      });
    });

    group('onViewsChanged', () {
      // Prepares a "timed-out" version of the onViewsChanged Stream so tests
      // can't hang "forever" waiting for this to complete. This stream will close
      // after 100ms of inactivity, regardless of what the test or the code do.
      final Stream<void> onViewsChanged = viewManager.onViewsChanged
        .timeout(const Duration(milliseconds: 100), onTimeout: (EventSink<void> sink) {
        sink.close();
      });

      test('on view registered/unregistered - fires event', () async {
        final EngineFlutterView view = EngineFlutterView(platformDispatcher, createDomElement('div'));
        final int viewId = view.viewId;

        final Future<List<void>> viewChangeEvents = onViewsChanged.toList();
        viewManager.registerView(view);
        viewManager.unregisterView(viewId);

        expect(viewChangeEvents, completes);

        final List<void> events = await viewChangeEvents;

        expect(events, hasLength(2), reason: 'Should fire once per viewManager register/unregister call.');
      });
    });
  });
}
