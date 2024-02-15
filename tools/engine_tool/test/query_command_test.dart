// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:convert' as convert;
import 'dart:ffi' as ffi show Abi;
import 'dart:io' as io;

import 'package:engine_build_configs/engine_build_configs.dart';
import 'package:engine_repo_tools/engine_repo_tools.dart';
import 'package:engine_tool/src/commands/command_runner.dart';
import 'package:engine_tool/src/environment.dart';
import 'package:litetest/litetest.dart';
import 'package:platform/platform.dart';
import 'package:process_fakes/process_fakes.dart';
import 'package:process_runner/process_runner.dart';

import 'fixtures.dart' as fixtures;

void main() {
  final Engine engine;
  try {
    engine = Engine.findWithin();
  } catch (e) {
    io.stderr.writeln(e);
    io.exitCode = 1;
    return;
  }

  final BuildConfig linuxTestConfig = BuildConfig.fromJson(
    path: 'ci/builders/linux_test_config.json',
    map: convert.jsonDecode(fixtures.testConfig('Linux')) as Map<String, Object?>,
  );

  final BuildConfig macTestConfig = BuildConfig.fromJson(
    path: 'ci/builders/mac_test_config.json',
    map: convert.jsonDecode(fixtures.testConfig('Mac-12')) as Map<String, Object?>,
  );

  final BuildConfig winTestConfig = BuildConfig.fromJson(
    path: 'ci/builders/win_test_config.json',
    map: convert.jsonDecode(fixtures.testConfig('Windows-11')) as Map<String, Object?>,
  );

  final Map<String, BuildConfig> configs = <String, BuildConfig>{
    'linux_test_config': linuxTestConfig,
    'linux_test_config2': linuxTestConfig,
    'mac_test_config': macTestConfig,
    'win_test_config': winTestConfig,
  };

  Environment linuxEnv(StringBuffer stderr, StringBuffer stdout) {
    return Environment(
      abi: ffi.Abi.linuxX64,
      engine: engine,
      platform: FakePlatform(operatingSystem: Platform.linux),
      processRunner: ProcessRunner(
        processManager: FakeProcessManager(),
      ),
      stderr: stderr,
      stdout: stdout,
    );
  }

  test('query command returns builds for the host platform.', () async {
    final StringBuffer stderr = StringBuffer();
    final StringBuffer stdout = StringBuffer();
    final Environment env = linuxEnv(stderr, stdout);
    final ToolCommandRunner runner = ToolCommandRunner(
      environment: env,
      configs: configs,
    );
    final int result = await runner.run(<String>[
      'query', 'builds',
    ]);
    expect(result, equals(0));
    expect(
      stdout.toString().trim().split('\n'),
      equals(<String>[
        'Add --verbose to see detailed information about each builder',
        '',
        '"linux_test_config" builder:',
        '  "build_name" config',
        '"linux_test_config2" builder:',
        '  "build_name" config',
      ]),
    );
  });

  test('query command with --builder returns only from the named builder.', () async {
    final StringBuffer stderr = StringBuffer();
    final StringBuffer stdout = StringBuffer();
    final Environment env = linuxEnv(stderr, stdout);
    final ToolCommandRunner runner = ToolCommandRunner(
      environment: env,
      configs: configs,
    );
    final int result = await runner.run(<String>[
      'query', 'builds', '--builder', 'linux_test_config',
    ]);
    expect(result, equals(0));
    expect(
      stdout.toString().trim().split('\n'),
      equals(<String>[
        'Add --verbose to see detailed information about each builder',
        '',
        '"linux_test_config" builder:',
        '  "build_name" config',
      ]),
    );
  });

  test('query command with --all returns all builds.', () async {
    final StringBuffer stderr = StringBuffer();
    final StringBuffer stdout = StringBuffer();
    final Environment env = linuxEnv(stderr, stdout);
    final ToolCommandRunner runner = ToolCommandRunner(
      environment: env,
      configs: configs,
    );
    final int result = await runner.run(<String>[
      'query', 'builds', '--all',
    ]);
    expect(result, equals(0));
    expect(
      stdout.toString().trim().split('\n').length,
      equals(10),
    );
  });
}
