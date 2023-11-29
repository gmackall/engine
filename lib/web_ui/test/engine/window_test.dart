// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:async';
import 'dart:js_interop';
import 'dart:js_util' as js_util;
import 'dart:typed_data';

import 'package:test/bootstrap/browser.dart';
import 'package:test/test.dart';
import 'package:ui/src/engine.dart';
import 'package:ui/ui.dart' as ui;

import '../common/matchers.dart';

const int kPhysicalKeyA = 0x00070004;
const int kLogicalKeyA = 0x00000000061;

void main() {
  internalBootstrapBrowserTest(() => testMain);
}

Future<void> testMain() async {
  late EngineFlutterWindow myWindow;
  final EnginePlatformDispatcher dispatcher = EnginePlatformDispatcher.instance;

  setUp(() {
    myWindow = EngineFlutterView.implicit(dispatcher, createDomHTMLDivElement());
    dispatcher.viewManager.registerView(myWindow);
  });

  tearDown(() async {
    dispatcher.viewManager.unregisterView(myWindow.viewId);
    await myWindow.resetHistory();
    myWindow.dispose();
  });

  test('onTextScaleFactorChanged preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback() {
        expect(Zone.current, innerZone);
      }
      myWindow.onTextScaleFactorChanged = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(myWindow.onTextScaleFactorChanged, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnTextScaleFactorChanged();
  });

  test('onPlatformBrightnessChanged preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback() {
        expect(Zone.current, innerZone);
      }
      myWindow.onPlatformBrightnessChanged = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(myWindow.onPlatformBrightnessChanged, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnPlatformBrightnessChanged();
  });

  test('onMetricsChanged preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback() {
        expect(Zone.current, innerZone);
      }
      myWindow.onMetricsChanged = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(myWindow.onMetricsChanged, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnMetricsChanged();
  });

  test('onLocaleChanged preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback() {
        expect(Zone.current, innerZone);
      }
      myWindow.onLocaleChanged = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(myWindow.onLocaleChanged, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnLocaleChanged();
  });

  test('onBeginFrame preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback(Duration _) {
        expect(Zone.current, innerZone);
      }
      myWindow.onBeginFrame = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(myWindow.onBeginFrame, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnBeginFrame(Duration.zero);
  });

  test('onReportTimings preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback(List<dynamic> _) {
        expect(Zone.current, innerZone);
      }
      myWindow.onReportTimings = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(myWindow.onReportTimings, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnReportTimings(<ui.FrameTiming>[]);
  });

  test('onDrawFrame preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback() {
        expect(Zone.current, innerZone);
      }
      myWindow.onDrawFrame = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(myWindow.onDrawFrame, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnDrawFrame();
  });

  test('onPointerDataPacket preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback(ui.PointerDataPacket _) {
        expect(Zone.current, innerZone);
      }
      myWindow.onPointerDataPacket = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(myWindow.onPointerDataPacket, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnPointerDataPacket(const ui.PointerDataPacket());
  });

  test('invokeOnKeyData returns normally when onKeyData is null', () {
    const  ui.KeyData keyData = ui.KeyData(
      timeStamp: Duration(milliseconds: 1),
      type: ui.KeyEventType.repeat,
      physical: kPhysicalKeyA,
      logical: kLogicalKeyA,
      character: 'a',
      synthesized: true,
    );
    expect(() {
      EnginePlatformDispatcher.instance.invokeOnKeyData(keyData, (bool result) {
        expect(result, isFalse);
      });
    }, returnsNormally);
  });

  test('onKeyData preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      bool onKeyData(ui.KeyData _) {
        expect(Zone.current, innerZone);
        return false;
      }
      myWindow.onKeyData = onKeyData;

      // Test that the getter returns the exact same onKeyData, e.g. it doesn't
      // wrap it.
      expect(myWindow.onKeyData, same(onKeyData));
    });

    const  ui.KeyData keyData = ui.KeyData(
      timeStamp: Duration(milliseconds: 1),
      type: ui.KeyEventType.repeat,
      physical: kPhysicalKeyA,
      logical: kLogicalKeyA,
      character: 'a',
      synthesized: true,
    );
    EnginePlatformDispatcher.instance.invokeOnKeyData(keyData, (bool result) {
      expect(result, isFalse);
    });

    myWindow.onKeyData = null;
  });

  test('onSemanticsEnabledChanged preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback() {
        expect(Zone.current, innerZone);
      }
      myWindow.onSemanticsEnabledChanged = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(myWindow.onSemanticsEnabledChanged, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnSemanticsEnabledChanged();
  });

  test('onSemanticsActionEvent preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback(ui.SemanticsActionEvent _) {
        expect(Zone.current, innerZone);
      }
      ui.PlatformDispatcher.instance.onSemanticsActionEvent = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(ui.PlatformDispatcher.instance.onSemanticsActionEvent, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnSemanticsAction(0, ui.SemanticsAction.tap, null);
  });

  test('onAccessibilityFeaturesChanged preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback() {
        expect(Zone.current, innerZone);
      }
      myWindow.onAccessibilityFeaturesChanged = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(myWindow.onAccessibilityFeaturesChanged, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnAccessibilityFeaturesChanged();
  });

  test('onPlatformMessage preserves the zone', () {
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      void callback(String _, ByteData? __, void Function(ByteData?)? ___) {
        expect(Zone.current, innerZone);
      }
      myWindow.onPlatformMessage = callback;

      // Test that the getter returns the exact same callback, e.g. it doesn't wrap it.
      expect(myWindow.onPlatformMessage, same(callback));
    });

    EnginePlatformDispatcher.instance.invokeOnPlatformMessage('foo', null, (ByteData? data) {
      // Not testing anything here.
    });
  });

  test('sendPlatformMessage preserves the zone', () async {
    final Completer<void> completer = Completer<void>();
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      final ByteData inputData = ByteData(4);
      inputData.setUint32(0, 42);
      myWindow.sendPlatformMessage(
        'flutter/debug-echo',
        inputData,
        (ByteData? outputData) {
          expect(Zone.current, innerZone);
          completer.complete();
        },
      );
    });

    await completer.future;
  });

  test('sendPlatformMessage responds even when channel is unknown', () async {
    bool responded = false;

    final ByteData inputData = ByteData(4);
    inputData.setUint32(0, 42);
    myWindow.sendPlatformMessage(
      'flutter/__unknown__channel__',
      null,
      (ByteData? outputData) {
        responded = true;
        expect(outputData, isNull);
      },
    );

    await Future<void>.delayed(const Duration(milliseconds: 1));
    expect(responded, isTrue);
  });

  // Emulates the framework sending a request for screen orientation lock.
  Future<bool> sendSetPreferredOrientations(List<dynamic> orientations) {
    final Completer<bool> completer = Completer<bool>();
    final ByteData? inputData = const JSONMethodCodec().encodeMethodCall(MethodCall(
      'SystemChrome.setPreferredOrientations',
      orientations,
    ));

    myWindow.sendPlatformMessage(
      'flutter/platform',
      inputData,
      (ByteData? outputData) {
        const MethodCodec codec = JSONMethodCodec();
        completer.complete(codec.decodeEnvelope(outputData!) as bool);
      },
    );

    return completer.future;
  }

  // Regression test for https://github.com/flutter/flutter/issues/88269
  test('sets preferred screen orientation', () async {
    final DomScreen? original = domWindow.screen;

    final List<String> lockCalls = <String>[];
    int unlockCount = 0;
    bool simulateError = false;

    // The `orientation` property cannot be overridden, so this test overrides the entire `screen`.
    js_util.setProperty(domWindow, 'screen', js_util.jsify(<Object?, Object?>{
      'orientation': <Object?, Object?>{
        'lock': (String lockType) {
          lockCalls.add(lockType);
          return futureToPromise(() async {
            if (simulateError) {
              throw Error();
            }
            return 0.toJS;
          }());
        }.toJS,
        'unlock': () {
          unlockCount += 1;
        }.toJS,
      },
    }));

    // Sanity-check the test setup.
    expect(lockCalls, <String>[]);
    expect(unlockCount, 0);
    await domWindow.screen!.orientation!.lock('hi');
    domWindow.screen!.orientation!.unlock();
    expect(lockCalls, <String>['hi']);
    expect(unlockCount, 1);
    lockCalls.clear();
    unlockCount = 0;

    expect(await sendSetPreferredOrientations(<dynamic>['DeviceOrientation.portraitUp']), isTrue);
    expect(lockCalls, <String>[ScreenOrientation.lockTypePortraitPrimary]);
    expect(unlockCount, 0);
    lockCalls.clear();
    unlockCount = 0;

    expect(await sendSetPreferredOrientations(<dynamic>['DeviceOrientation.portraitDown']), isTrue);
    expect(lockCalls, <String>[ScreenOrientation.lockTypePortraitSecondary]);
    expect(unlockCount, 0);
    lockCalls.clear();
    unlockCount = 0;

    expect(await sendSetPreferredOrientations(<dynamic>['DeviceOrientation.landscapeLeft']), isTrue);
    expect(lockCalls, <String>[ScreenOrientation.lockTypeLandscapePrimary]);
    expect(unlockCount, 0);
    lockCalls.clear();
    unlockCount = 0;

    expect(await sendSetPreferredOrientations(<dynamic>['DeviceOrientation.landscapeRight']), isTrue);
    expect(lockCalls, <String>[ScreenOrientation.lockTypeLandscapeSecondary]);
    expect(unlockCount, 0);
    lockCalls.clear();
    unlockCount = 0;

    expect(await sendSetPreferredOrientations(<dynamic>[]), isTrue);
    expect(lockCalls, <String>[]);
    expect(unlockCount, 1);
    lockCalls.clear();
    unlockCount = 0;

    simulateError = true;
    expect(await sendSetPreferredOrientations(<dynamic>['DeviceOrientation.portraitDown']), isFalse);
    expect(lockCalls, <String>[ScreenOrientation.lockTypePortraitSecondary]);
    expect(unlockCount, 0);

    js_util.setProperty(domWindow, 'screen', original);
  });

  /// Regression test for https://github.com/flutter/flutter/issues/66128.
  test("setPreferredOrientation responds even if browser doesn't support api", () async {
    final DomScreen? original = domWindow.screen;

    // The `orientation` property cannot be overridden, so this test overrides the entire `screen`.
    js_util.setProperty(domWindow, 'screen', js_util.jsify(<Object?, Object?>{
      'orientation': null,
    }));
    expect(domWindow.screen!.orientation, isNull);
    expect(await sendSetPreferredOrientations(<dynamic>[]), isFalse);
    js_util.setProperty(domWindow, 'screen', original);
  });

  test('SingletonFlutterWindow implements locale, locales, and locale change notifications', () async {
    // This will count how many times we notified about locale changes.
    int localeChangedCount = 0;
    myWindow.onLocaleChanged = () {
      localeChangedCount += 1;
    };

    // We populate the initial list of locales automatically (only test that we
    // got some locales; some contributors may be in different locales, so we
    // can't test the exact contents).
    expect(myWindow.locale, isA<ui.Locale>());
    expect(myWindow.locales, isNotEmpty);

    // Trigger a change notification (reset locales because the notification
    // doesn't actually change the list of languages; the test only observes
    // that the list is populated again).
    EnginePlatformDispatcher.instance.debugResetLocales();
    expect(myWindow.locales, isEmpty);
    expect(myWindow.locale, equals(const ui.Locale.fromSubtags()));
    expect(localeChangedCount, 0);
    domWindow.dispatchEvent(createDomEvent('Event', 'languagechange'));
    expect(myWindow.locales, isNotEmpty);
    expect(localeChangedCount, 1);
  });

  test('dispatches browser event on flutter/service_worker channel', () async {
    final Completer<void> completer = Completer<void>();
    domWindow.addEventListener('flutter-first-frame',
        createDomEventListener((DomEvent e) => completer.complete()));
    final Zone innerZone = Zone.current.fork();

    innerZone.runGuarded(() {
      myWindow.sendPlatformMessage(
        'flutter/service_worker',
        ByteData(0),
        (ByteData? outputData) { },
      );
    });

    await expectLater(completer.future, completes);
  });

  test('auto-view-id', () {
    final DomElement host = createDomHTMLDivElement();
    final EngineFlutterView implicit1 = EngineFlutterView.implicit(dispatcher, host);
    final EngineFlutterView implicit2 = EngineFlutterView.implicit(dispatcher, host);

    expect(implicit1.viewId, kImplicitViewId);
    expect(implicit2.viewId, kImplicitViewId);

    final EngineFlutterView view1 = EngineFlutterView(dispatcher, host);
    final EngineFlutterView view2 = EngineFlutterView(dispatcher, host);
    final EngineFlutterView view3 = EngineFlutterView(dispatcher, host);

    expect(view1.viewId, isNot(kImplicitViewId));
    expect(view2.viewId, isNot(kImplicitViewId));
    expect(view3.viewId, isNot(kImplicitViewId));

    expect(view1.viewId, isNot(view2.viewId));
    expect(view2.viewId, isNot(view3.viewId));
    expect(view3.viewId, isNot(view1.viewId));

    implicit1.dispose();
    implicit2.dispose();
    view1.dispose();
    view2.dispose();
    view3.dispose();
  });

  test('registration', () {
    final DomHTMLDivElement host = createDomHTMLDivElement();
    final EnginePlatformDispatcher dispatcher = EnginePlatformDispatcher();
    expect(dispatcher.viewManager.views, isEmpty);

    // Creating the view shouldn't register it.
    final EngineFlutterView view = EngineFlutterView(dispatcher, host);
    expect(dispatcher.viewManager.views, isEmpty);
    dispatcher.viewManager.registerView(view);
    expect(dispatcher.viewManager.views, <EngineFlutterView>[view]);

    // Disposing the view shouldn't unregister it.
    view.dispose();
    expect(dispatcher.viewManager.views, <EngineFlutterView>[view]);

    dispatcher.dispose();
  });

  test('dispose', () {
    final DomHTMLDivElement host = createDomHTMLDivElement();
    final EngineFlutterView view =
        EngineFlutterView(EnginePlatformDispatcher.instance, host);

    // First, let's make sure the view's root element was inserted into the
    // host, and the dimensions provider is active.
    expect(view.dom.rootElement.parentElement, host);
    expect(view.dimensionsProvider.isClosed, isFalse);

    // Now, let's dispose the view and make sure its root element was removed,
    // and the dimensions provider is closed.
    view.dispose();
    expect(view.dom.rootElement.parentElement, isNull);
    expect(view.dimensionsProvider.isClosed, isTrue);

    // Can't render into a disposed view.
    expect(
      () => view.render(ui.SceneBuilder().build()),
      throwsAssertionError,
    );

    // Can't update semantics on a disposed view.
    expect(
      () => view.updateSemantics(ui.SemanticsUpdateBuilder().build()),
      throwsAssertionError,
    );
  });
}
