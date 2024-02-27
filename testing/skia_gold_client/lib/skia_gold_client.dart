// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:convert';
import 'dart:io' as io;

import 'package:crypto/crypto.dart';
import 'package:engine_repo_tools/engine_repo_tools.dart';
import 'package:meta/meta.dart';
import 'package:path/path.dart' as path;
import 'package:process/process.dart';

const String _kGoldctlKey = 'GOLDCTL';
const String _kPresubmitEnvName = 'GOLD_TRYJOB';
const String _kLuciEnvName = 'LUCI_CONTEXT';

const String _skiaGoldHost = 'https://flutter-engine-gold.skia.org';
const String _instance = 'flutter-engine';

/// Whether the Skia Gold client is available and can be used in this
/// environment.
bool get isSkiaGoldClientAvailable => SkiaGoldClient.isAvailable();

/// Returns true if the current environment is a LUCI builder.
bool get isLuciEnv => io.Platform.environment.containsKey(_kLuciEnvName);

/// A client for uploading image tests and making baseline requests to the
/// Flutter Gold Dashboard.
interface class SkiaGoldClient {
  /// Creates a [SkiaGoldClient] with the given [workDirectory].
  ///
  /// [dimensions] allows to add attributes about the environment
  /// used to generate the screenshots.
  SkiaGoldClient(
    this.workDirectory, {
    this.dimensions,
    this.verbose = false,
    io.HttpClient? httpClient,
    ProcessManager? processManager,
    StringSink? stderr,
    Map<String, String>? environment,
  }) : httpClient = httpClient ?? io.HttpClient(),
       process = processManager ?? const LocalProcessManager(),
       _stderr = stderr ?? io.stderr,
        _environment = environment ?? io.Platform.environment;

  /// Whether the client is available and can be used in this environment.
  static bool isAvailable({
    Map<String, String>? environment,
  }) {
    final String? result = (environment ?? io.Platform.environment)[_kGoldctlKey];
    return result != null && result.isNotEmpty;
  }

  /// Returns true if the current environment is a LUCI builder.
  static bool isLuciEnv({
    Map<String, String>? environment,
  }) {
    return (environment ?? io.Platform.environment).containsKey(_kLuciEnvName);
  }

  /// Whether the current environment is a presubmit job.
  bool get _isPresubmit {
    return
        isLuciEnv(environment: _environment) &&
        isAvailable(environment: _environment) &&
        _environment.containsKey(_kPresubmitEnvName);
  }

  /// Whether the current environment is a postsubmit job.
  bool get _isPostsubmit {
    return
        isLuciEnv(environment: _environment) &&
        isAvailable(environment: _environment) &&
        !_environment.containsKey(_kPresubmitEnvName);
  }

  /// Whether to print verbose output from goldctl.
  ///
  /// This flag is intended for use in debugging CI issues, and should not
  /// ordinarily be set to true.
  final bool verbose;

  /// Environment variables for the currently running process.
  final Map<String, String> _environment;

  /// Where output is written for diagnostics.
  final StringSink _stderr;

  /// Allows to add attributes about the environment used to generate the screenshots.
  final Map<String, String>? dimensions;

  /// A controller for launching sub-processes.
  final ProcessManager process;

  /// A client for making Http requests to the Flutter Gold dashboard.
  final io.HttpClient httpClient;

  /// The local [Directory] for the current test context. In this directory, the
  /// client will create image and JSON files for the `goldctl` tool to use.
  final io.Directory workDirectory;

  String get _tempPath => path.join(workDirectory.path, 'temp');
  String get _keysPath => path.join(workDirectory.path, 'keys.json');
  String get _failuresPath => path.join(workDirectory.path, 'failures.json');

  Future<void>? _initResult;
  Future<void> _initOnce(Future<void> Function() callback) {
    // If a call has already been made, return the result of that call.
    _initResult ??= callback();
    return _initResult!;
  }

  /// Indicates whether the client has already been authorized to communicate
  /// with the Skia Gold backend.
  bool get _isAuthorized {
    final io.File authFile = io.File(path.join(_tempPath, 'auth_opt.json'));

    if (authFile.existsSync()) {
      final String contents = authFile.readAsStringSync();
      final Map<String, dynamic> decoded = json.decode(contents) as Map<String, dynamic>;
      return !(decoded['GSUtil'] as bool);
    }
    return false;
  }

  /// The path to the local [Directory] where the `goldctl` tool is hosted.
  String get _goldctl {
    assert(
      isAvailable(environment: _environment),
      'Trying to use `goldctl` in an environment where it is not available',
    );
    final String? result = _environment[_kGoldctlKey];
    if (result == null || result.isEmpty) {
      throw StateError('The environment variable $_kGoldctlKey is not set.');
    }
    return result;
  }

  /// Prepares the local work space for golden file testing and calls the
  /// `goldctl auth` command.
  ///
  /// This ensures that the `goldctl` tool is authorized and ready for testing.
  Future<void> auth() async {
    if (_isAuthorized) {
      return;
    }
    final List<String> authCommand = <String>[
      _goldctl,
      'auth',
      if (verbose) '--verbose',
      '--work-dir', _tempPath,
      '--luci',
    ];

    final io.ProcessResult result = await _runCommand(authCommand);

    if (result.exitCode != 0) {
      final StringBuffer buf = StringBuffer()
        ..writeln('Skia Gold authorization failed.')
        ..writeln('Luci environments authenticate using the file provided '
          'by LUCI_CONTEXT. There may be an error with this file or Gold '
          'authentication.')
        ..writeln('Debug information for Gold:')
        ..writeln('stdout: ${result.stdout}')
        ..writeln('stderr: ${result.stderr}');
      throw Exception(buf.toString());
    } else if (verbose) {
      _stderr.writeln('stdout:\n${result.stdout}');
      _stderr.writeln('stderr:\n${result.stderr}');
    }
  }

  Future<io.ProcessResult> _runCommand(List<String> command) {
    return process.run(command);
  }

  /// Executes the `imgtest init` command in the `goldctl` tool.
  ///
  /// The `imgtest` command collects and uploads test results to the Skia Gold
  /// backend, the `init` argument initializes the current test.
  Future<void> _imgtestInit() async {
    final io.File keys = io.File(_keysPath);
    final io.File failures = io.File(_failuresPath);

    await keys.writeAsString(_getKeysJSON());
    await failures.create();
    final String commitHash = await _getCurrentCommit();

    final List<String> imgtestInitCommand = <String>[
      _goldctl,
      'imgtest', 'init',
      if (verbose) '--verbose',
      '--instance', _instance,
      '--work-dir', _tempPath,
      '--commit', commitHash,
      '--keys-file', keys.path,
      '--failure-file', failures.path,
      '--passfail',
    ];

    if (imgtestInitCommand.contains(null)) {
      final StringBuffer buf = StringBuffer()
        ..writeln('A null argument was provided for Skia Gold imgtest init.')
        ..writeln('Please confirm the settings of your golden file test.')
        ..writeln('Arguments provided:');
      imgtestInitCommand.forEach(buf.writeln);
      throw Exception(buf.toString());
    }

    final io.ProcessResult result = await _runCommand(imgtestInitCommand);

    if (result.exitCode != 0) {
      final StringBuffer buf = StringBuffer()
        ..writeln('Skia Gold imgtest init failed.')
        ..writeln('An error occurred when initializing golden file test with ')
        ..writeln('goldctl.')
        ..writeln()
        ..writeln('Debug information for Gold:')
        ..writeln('stdout: ${result.stdout}')
        ..writeln('stderr: ${result.stderr}');
      throw Exception(buf.toString());
    } else if (verbose) {
      _stderr.writeln('stdout:\n${result.stdout}');
      _stderr.writeln('stderr:\n${result.stderr}');
    }

  }

  /// Executes the `imgtest add` command in the `goldctl` tool.
  ///
  /// The `imgtest` command collects and uploads test results to the Skia Gold
  /// backend, the `add` argument uploads the current image test.
  ///
  /// Throws an exception for try jobs that failed to pass the pixel comparison.
  ///
  /// The [testName] and [goldenFile] parameters reference the current
  /// comparison being evaluated.
  ///
  /// [pixelColorDelta] defines maximum acceptable difference in RGB channels of
  /// each pixel, such that:
  ///
  /// ```
  /// bool isSame(Color image, Color golden, int pixelDeltaThreshold) {
  ///   return abs(image.r - golden.r)
  ///     + abs(image.g - golden.g)
  ///     + abs(image.b - golden.b) <= pixelDeltaThreshold;
  /// }
  /// ```
  ///
  /// [differentPixelsRate] is the fraction of pixels that can differ, as
  /// determined by the [pixelColorDelta] parameter. It's in the range [0.0,
  /// 1.0] and defaults to 0.01. A value of 0.01 means that 1% of the pixels are
  /// allowed to be different.
  Future<void> addImg(
    String testName,
    io.File goldenFile, {
    double differentPixelsRate = 0.01,
    int pixelColorDelta = 0,
    required int screenshotSize,
  }) async {
    assert(_isPresubmit || _isPostsubmit);
    if (_isPresubmit) {
      await _tryjobAdd(testName, goldenFile, screenshotSize, pixelColorDelta, differentPixelsRate);
    }
    if (_isPostsubmit) {
      await _imgtestAdd(testName, goldenFile, screenshotSize, pixelColorDelta, differentPixelsRate);
    }
  }

  /// Executes the `imgtest add` command in the `goldctl` tool.
  ///
  /// The `imgtest` command collects and uploads test results to the Skia Gold
  /// backend, the `add` argument uploads the current image test. A response is
  /// returned from the invocation of this command that indicates a pass or fail
  /// result.
  ///
  /// The [testName] and [goldenFile] parameters reference the current
  /// comparison being evaluated.
  Future<void> _imgtestAdd(
    String testName,
    io.File goldenFile,
    int screenshotSize,
    int pixelDeltaThreshold,
    double maxDifferentPixelsRate,
  ) async {
    await _initOnce(_imgtestInit);

    final List<String> imgtestCommand = <String>[
      _goldctl,
      'imgtest',
      'add',
      if (verbose)
      '--verbose',
      '--work-dir',
      _tempPath,
      '--test-name',
      _cleanTestName(testName),
      '--png-file',
      goldenFile.path,
      // Otherwise post submit will not fail.
      '--passfail',
      ..._getMatchingArguments(testName, screenshotSize, pixelDeltaThreshold, maxDifferentPixelsRate),
    ];

    final io.ProcessResult result = await _runCommand(imgtestCommand);

    if (result.exitCode != 0) {
      final StringBuffer buf = StringBuffer()
        ..writeln('Skia Gold received an unapproved image in post-submit ')
        ..writeln('testing. Golden file images in flutter/engine are triaged ')
        ..writeln('in pre-submit during code review for the given PR.')
        ..writeln()
        ..writeln('Visit https://flutter-engine-gold.skia.org/ to view and approve ')
        ..writeln('the image(s), or revert the associated change. For more ')
        ..writeln('information, visit the wiki: ')
        ..writeln('https://github.com/flutter/flutter/wiki/Writing-a-golden-file-test-for-package:flutter')
        ..writeln()
        ..writeln('Debug information for Gold --------------------------------')
        ..writeln('stdout: ${result.stdout}')
        ..writeln('stderr: ${result.stderr}');
      throw Exception(buf.toString());
    } else if (verbose) {
      _stderr.writeln('stdout:\n${result.stdout}');
      _stderr.writeln('stderr:\n${result.stderr}');
    }
  }

  /// Executes the `imgtest init` command in the `goldctl` tool for tryjobs.
  ///
  /// The `imgtest` command collects and uploads test results to the Skia Gold
  /// backend, the `init` argument initializes the current tryjob.
  Future<void> _tryjobInit() async {
    final io.File keys = io.File(_keysPath);
    final io.File failures = io.File(_failuresPath);

    await keys.writeAsString(_getKeysJSON());
    await failures.create();
    final String commitHash = await _getCurrentCommit();

    final List<String> tryjobInitCommand = <String>[
      _goldctl,
      'imgtest', 'init',
      if (verbose) '--verbose',
      '--instance', _instance,
      '--work-dir', _tempPath,
      '--commit', commitHash,
      '--keys-file', keys.path,
      '--failure-file', failures.path,
      '--passfail',
      '--crs', 'github',
      '--patchset_id', commitHash,
      ..._getCIArguments(),
    ];

    if (tryjobInitCommand.contains(null)) {
      final StringBuffer buf = StringBuffer()
        ..writeln('A null argument was provided for Skia Gold tryjob init.')
        ..writeln('Please confirm the settings of your golden file test.')
        ..writeln('Arguments provided:');
      tryjobInitCommand.forEach(buf.writeln);
      throw Exception(buf.toString());
    }

    final io.ProcessResult result = await _runCommand(tryjobInitCommand);

    if (result.exitCode != 0) {
      final StringBuffer buf = StringBuffer()
        ..writeln('Skia Gold tryjobInit failure.')
        ..writeln('An error occurred when initializing golden file tryjob with ')
        ..writeln('goldctl.')
        ..writeln()
        ..writeln('Debug information for Gold:')
        ..writeln('stdout: ${result.stdout}')
        ..writeln('stderr: ${result.stderr}');
      throw Exception(buf.toString());
    } else if (verbose) {
      _stderr.writeln('stdout:\n${result.stdout}');
      _stderr.writeln('stderr:\n${result.stderr}');
    }
  }

  /// Executes the `imgtest add` command in the `goldctl` tool for tryjobs.
  ///
  /// The `imgtest` command collects and uploads test results to the Skia Gold
  /// backend, the `add` argument uploads the current image test. A response is
  /// returned from the invocation of this command that indicates a pass or fail
  /// result for the tryjob.
  ///
  /// The [testName] and [goldenFile] parameters reference the current
  /// comparison being evaluated.
  Future<void> _tryjobAdd(
    String testName,
    io.File goldenFile,
    int screenshotSize,
    int pixelDeltaThreshold,
    double differentPixelsRate,
  ) async {
    await _initOnce(_tryjobInit);

    final List<String> tryjobCommand = <String>[
      _goldctl,
      'imgtest',
      'add',
      if (verbose) '--verbose',
      '--work-dir',
      _tempPath,
      '--test-name',
      _cleanTestName(testName),
      '--png-file',
      goldenFile.path,
      ..._getMatchingArguments(testName, screenshotSize, pixelDeltaThreshold, differentPixelsRate),
    ];

    final io.ProcessResult result = await _runCommand(tryjobCommand);

    final String resultStdout = result.stdout.toString();
    if (result.exitCode != 0 &&
      !(resultStdout.contains('Untriaged') || resultStdout.contains('negative image'))) {
      final StringBuffer buf = StringBuffer()
        ..writeln('Unexpected Gold tryjobAdd failure.')
        ..writeln('Tryjob execution for golden file test $testName failed for')
        ..writeln('a reason unrelated to pixel comparison.')
        ..writeln()
        ..writeln('Debug information for Gold:')
        ..writeln('stdout: ${result.stdout}')
        ..writeln('stderr: ${result.stderr}')
        ..writeln();
      throw Exception(buf.toString());
    } else if (verbose) {
      _stderr.writeln('stdout:\n${result.stdout}');
      _stderr.writeln('stderr:\n${result.stderr}');
    }
  }

  List<String> _getMatchingArguments(
    String testName,
    int screenshotSize,
    int pixelDeltaThreshold,
    double differentPixelsRate,
  ) {
    // The algorithm to be used when matching images. The available options are:
    // - "fuzzy": Allows for customizing the thresholds of pixel differences.
    // - "sobel": Same as "fuzzy" but performs edge detection before performing
    //            a fuzzy match.
    const String algorithm = 'fuzzy';

    // The number of pixels in this image that are allowed to differ from the
    // baseline. It's okay for this to be a slightly high number like 10% of the
    // image size because those wrong pixels are constrained by
    // `pixelDeltaThreshold` below.
    final int maxDifferentPixels = (screenshotSize * differentPixelsRate).toInt();
    return <String>[
      '--add-test-optional-key', 'image_matching_algorithm:$algorithm',
      '--add-test-optional-key', 'fuzzy_max_different_pixels:$maxDifferentPixels',
      '--add-test-optional-key', 'fuzzy_pixel_delta_threshold:$pixelDeltaThreshold',
    ];
  }

  /// Returns the latest positive digest for the given test known to Skia Gold
  /// at head.
  Future<String?> getExpectationForTest(String testName) async {
    late String? expectation;
    final String traceID = getTraceID(testName);
    await io.HttpOverrides.runWithHttpOverrides<Future<void>>(() async {
      final Uri requestForExpectations = Uri.parse(
        '$_skiaGoldHost/json/v2/latestpositivedigest/$traceID'
      );
      late String rawResponse;
      try {
        final io.HttpClientRequest request = await httpClient.getUrl(requestForExpectations);
        final io.HttpClientResponse response = await request.close();
        rawResponse = await utf8.decodeStream(response);
        final dynamic jsonResponse = json.decode(rawResponse);
        if (jsonResponse is! Map<String, dynamic>) {
          throw const FormatException('Skia gold expectations do not match expected format.');
        }
        expectation = jsonResponse['digest'] as String?;
      } on FormatException catch (error) {
        _stderr.writeln(
          'Formatting error detected requesting expectations from Flutter Gold.\n'
          'error: $error\n'
          'url: $requestForExpectations\n'
          'response: $rawResponse'
        );
        rethrow;
      }
    },
      SkiaGoldHttpOverrides(),
    );
    return expectation;
  }

  /// Returns the current commit hash of the engine repository.
  Future<String> _getCurrentCommit() async {
    final String engineCheckout = Engine.findWithin().flutterDir.path;
    final io.ProcessResult revParse = await process.run(
      <String>['git', 'rev-parse', 'HEAD'],
      workingDirectory: engineCheckout,
    );
    if (revParse.exitCode != 0) {
      throw Exception('Current commit of the engine can not be found from path $engineCheckout.');
    }
    return (revParse.stdout as String).trim();
  }

  /// Returns a Map of key value pairs used to uniquely identify the
  /// configuration that generated the given golden file.
  ///
  /// Currently, the only key value pairs being tracked are the platform and
  /// browser the image was rendered on.
  Map<String, dynamic> _getKeys() {
    final Map<String, dynamic> initialKeys = <String, dynamic>{
      'CI': 'luci',
      'Platform': io.Platform.operatingSystem,
    };
    if (dimensions != null) {
      initialKeys.addAll(dimensions!);
    }
    return initialKeys;
  }

  /// Same as [_getKeys] but encodes it in a JSON string.
  String _getKeysJSON() {
    return json.encode(_getKeys());
  }

  /// Removes the file extension from the [fileName] to represent the test name
  /// properly.
  static String _cleanTestName(String fileName) {
    return path.basenameWithoutExtension(fileName);
  }

  /// Returns a list of arguments for initializing a tryjob based on the testing
  /// environment.
  List<String> _getCIArguments() {
    final String jobId = _environment['LOGDOG_STREAM_PREFIX']!.split('/').last;
    final List<String> refs = _environment['GOLD_TRYJOB']!.split('/');
    final String pullRequest = refs[refs.length - 2];

    return <String>[
      '--changelist', pullRequest,
      '--cis', 'buildbucket',
      '--jobid', jobId,
    ];
  }

  /// Returns a trace id based on the current testing environment to lookup
  /// the latest positive digest on Skia Gold with a hex-encoded md5 hash of
  /// the image keys.
  @visibleForTesting
  String getTraceID(String testName) {
    final Map<String, dynamic> keys = <String, dynamic>{
      ..._getKeys(),
      'name': testName,
      'source_type': _instance,
    };
    final String jsonTrace = json.encode(keys);
    final String md5Sum = md5.convert(utf8.encode(jsonTrace)).toString();
    return md5Sum;
  }
}

/// Used to make HttpRequests during testing.
class SkiaGoldHttpOverrides extends io.HttpOverrides { }
