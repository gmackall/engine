# Scenario App: Android Tests

As mentioned in the [top-level README](../README.md), this directory contains
the Android-specific native code and tests for the [scenario app](../lib). To
run the tests, you will need to build the engine with the appropriate
configuration.

For example, for the latest `android` build you've made locally:

```sh
dart ./testing/scenario_app/bin/run_android_tests.dart
```

Or for a specific, build, such as `android_debug_unopt_arm64`:

```sh
dart ./testing/scenario_app/bin/run_android_tests.dart --out-dir=../out/android_debug_unopt_arm64
```

## Debugging

Debugging the tests on CI is not straightforward but is being improved:

- <https://github.com/flutter/flutter/issues/143458>
- <https://github.com/flutter/flutter/issues/143459>

Locally (or on a temporary PR for CI), you can run the tests with the
`--smoke-test` argument to run a single test by class name, which can be useful
to verify the setup:

```sh
dart ./testing/scenario_app/bin/run_android_tests.dart --smoke-test dev.flutter.scenarios.EngineLaunchE2ETest
```

The result of `adb logcat` and screenshots taken during the test will be stored
in a logs directory, which is either `FLUTTER_LOGS_DIR` (if set, such as on CI)
or locally in `out/.../scenario_app/logs`.

You can then view the logs and screenshots on LUCI. [For example](https://ci.chromium.org/ui/p/flutter/builders/try/Linux%20Engine%20Drone/2003164/overview):

![Screenshot of the Logs on LUCI](https://github.com/flutter/engine/assets/168174/79dc864c-c18b-4df9-a733-fd55301cc69c).

## CI Configuration

See [`ci/builders/linux_android_emulator.json`](../../../ci/builders/linux_android_emulator.json)
, and grep for `run_android_tests.dart`.

The following matrix of configurations is tested on the CI:

<!-- TODO(matanlurey): Blocked by https://github.com/flutter/flutter/issues/143471.
| 28          | Skia                | [Android 28 + Skia][skia-gold-skia-28]                           | Older Android devices (without `ImageReader`) on Skia.     |
| 28          | Impeller (OpenGLES) | [Android 28 + Impeller OpenGLES][skia-gold-impeller-opengles-28] | Older Android devices (without `ImageReader`) on Impeller. |
[skia-gold-skia-28]: https://flutter-engine-gold.skia.org/search?left_filter=AndroidAPILevel%3D28%26GraphicsBackend%3Dskia&negative=true&positive=true&right_filter=AndroidAPILevel%3D28%26GraphicsBackend%3Dskia
[skia-gold-impeller-opengles-28]: https://flutter-engine-gold.skia.org/search?left_filter=AndroidAPILevel%3D28%26GraphicsBackend%3Dimpeller-opengles&negative=true&positive=true&right_filter=AndroidAPILevel%3D28%26GraphicsBackend%3Dimpeller-opengles
-->

| API Version | Graphics Backend    | Skia Gold                                                        | Rationale                                        |
| ----------- | ------------------- | ---------------------------------------------------------------- | ------------------------------------------------ |
| 34          | Skia                | [Android 34 + Skia][skia-gold-skia-34]                           | Newer Android devices on Skia.                   |
| 34          | Impeller (OpenGLES) | [Android 34 + Impeller OpenGLES][skia-gold-impeller-opengles-34] | Newer Android devices on Impeller with OpenGLES. |
| 34          | Impeller (Vulkan)   | [Android 34 + Impeller Vulkan][skia-gold-impeller-vulkan-34]     | Newer Android devices on Impeller.               |

[skia-gold-skia-34]: https://flutter-engine-gold.skia.org/search?left_filter=AndroidAPILevel%3D34%26GraphicsBackend%3Dskia&negative=true&positive=true&right_filter=AndroidAPILevel%3D34%26GraphicsBackend%3Dskia
[skia-gold-impeller-opengles-34]: https://flutter-engine-gold.skia.org/search?left_filter=AndroidAPILevel%3D34%26GraphicsBackend%3Dimpeller-opengles&negative=true&positive=true&right_filter=AndroidAPILevel%3D34%26GraphicsBackend%3Dimpeller-opengles
[skia-gold-impeller-vulkan-34]: https://flutter-engine-gold.skia.org/search?left_filter=AndroidAPILevel%3D34%26GraphicsBackend%3Dimpeller-vulkan&negative=true&positive=true&right_filter=AndroidAPILevel%3D34%26GraphicsBackend%3Dimpeller-vulkan

## Updating Gradle dependencies

See [Updating the Embedding Dependencies](../../../tools/cipd/android_embedding_bundle/README.md).

## Output validation

The generated output will be checked against a golden file
([`expected_golden_output.txt`](./expected_golden_output.txt)) to make sure all
output was generated. A patch will be printed to stdout if they don't match.
