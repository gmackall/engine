// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'package:args/command_runner.dart';

import 'package:engine_build_configs/engine_build_configs.dart';

import '../environment.dart';
import 'query_command.dart';

/// The root command runner.
final class ToolCommandRunner extends CommandRunner<int> {
  /// Constructs the runner and populates commands, subcommands, and global
  /// options and flags.
  ToolCommandRunner({
    required this.environment,
    required this.configs,
  }) : super(toolName, toolDescription) {
    addCommand(QueryCommand(
      environment: environment,
      configs: configs,
    ));
  }

  /// The name of the tool as reported in the tool's usage and help
  /// messages.
  static const String toolName = 'et';

  /// The description of the tool reported in the tool's usage and help
  /// messages.
  static const String toolDescription = 'A command line tool for working on '
                                        'the Flutter Engine.';

  /// The host system environment.
  final Environment environment;

  /// Build configurations loaded from the engine from under ci/builders.
  final Map<String, BuildConfig> configs;

  @override
  Future<int> run(Iterable<String> args) async {
    try{
      return await runCommand(parse(args)) ?? 1;
    } on FormatException catch (e) {
      environment.stderr.writeln(e);
      return 1;
    } on UsageException catch (e) {
      environment.stderr.writeln(e);
      return 1;
    }
  }
}
