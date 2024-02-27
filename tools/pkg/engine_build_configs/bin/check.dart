// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:io' as io;

import 'package:engine_build_configs/engine_build_configs.dart';
import 'package:engine_repo_tools/engine_repo_tools.dart';
import 'package:path/path.dart' as p;
import 'package:platform/platform.dart';

// Usage:
// $ dart bin/check.dart [/path/to/engine/src]

void main(List<String> args) {
  final String? engineSrcPath;
  if (args.isNotEmpty) {
    engineSrcPath = args[0];
  } else {
    engineSrcPath = null;
  }

  // Find the engine repo.
  final Engine engine;
  try {
    engine = Engine.findWithin(engineSrcPath);
  } catch (e) {
    io.stderr.writeln(e);
    io.exitCode = 1;
    return;
  }

  // Find and parse the engine build configs.
  final io.Directory buildConfigsDir = io.Directory(p.join(
    engine.flutterDir.path, 'ci', 'builders',
  ));
  final BuildConfigLoader loader = BuildConfigLoader(
    buildConfigsDir: buildConfigsDir,
  );

  // Treat it as an error if no build configs were found. The caller likely
  // expected to find some.
  final Map<String, BuildConfig> configs = loader.configs;
  if (configs.isEmpty) {
    io.stderr.writeln(
      'Error: No build configs found under ${buildConfigsDir.path}',
    );
    io.exitCode = 1;
    return;
  }
  if (loader.errors.isNotEmpty) {
    loader.errors.forEach(io.stderr.writeln);
    io.exitCode = 1;
  }

  // Check the parsed build configs for validity.
  for (final String name in configs.keys) {
    final BuildConfig buildConfig = configs[name]!;
    final List<String> buildConfigErrors = buildConfig.check(name);
    if (buildConfigErrors.isNotEmpty) {
      io.stderr.writeln('Errors in ${buildConfig.path}:');
      io.exitCode = 1;
    }
    for (final String error in buildConfigErrors) {
      io.stderr.writeln('    $error');
    }
  }

  // Check that global builds for the same platform are uniquely named.
  final List<String> hostPlatforms = <String>[
    Platform.linux, Platform.macOS, Platform.windows,
  ];

  // For each host platform, a mapping from GlobalBuild.name to GlobalBuild.
  final Map<String, Map<String, (BuildConfig, GlobalBuild)>> builds =
    <String, Map<String, (BuildConfig, GlobalBuild)>>{};
  for (final String platform in hostPlatforms) {
    builds[platform] = <String, (BuildConfig, GlobalBuild)>{};
  }
  for (final String name in configs.keys) {
    final BuildConfig buildConfig = configs[name]!;
    for (final GlobalBuild build in buildConfig.builds) {
      for (final String platform in hostPlatforms) {
        if (!build.canRunOn(FakePlatform(operatingSystem: platform))) {
          continue;
        }
        if (builds[platform]!.containsKey(build.name)) {
          final (BuildConfig oldConfig, _) = builds[platform]![build.name]!;
          io.stderr.writeln(
            '${build.name} is duplicated.\n'
            '\tFound definition in ${buildConfig.path}.\n'
            '\tPrevious definition was in ${oldConfig.path}.',
          );
          io.exitCode = 1;
        }
        builds[platform]![build.name] = (buildConfig, build);
      }
    }
  }
}
