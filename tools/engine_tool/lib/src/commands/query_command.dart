// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'package:engine_build_configs/engine_build_configs.dart';

import 'command.dart';

const String _allFlag = 'all';
const String _builderFlag = 'builder';
const String _verboseFlag = 'verbose';

/// The root 'query' command.
final class QueryCommand extends CommandBase {
  /// Constructs the 'query' command.
  QueryCommand({
    required super.environment,
    required this.configs,
  }) {
    // Add options here that are common to all queries.
    argParser
      ..addFlag(
        _allFlag,
        abbr: 'a',
        help: 'List all results, even when not relevant on this platform',
        negatable: false,
      )
      ..addOption(
        _builderFlag,
        abbr: 'b',
        help: 'Restrict the query to a single builder.',
        allowed: <String>[
          for (final MapEntry<String, BuildConfig> entry in configs.entries)
            if (entry.value.canRunOn(environment.platform))
              entry.key,
        ],
        allowedHelp: <String, String>{
          // TODO(zanderso): Add human readable descriptions to the json files.
          for (final MapEntry<String, BuildConfig> entry in configs.entries)
            if (entry.value.canRunOn(environment.platform))
              entry.key: entry.value.path,
        },
      )
      ..addFlag(
        _verboseFlag,
        abbr: 'v',
        help: 'Respond to queries with extra information',
        negatable: false,
      );

    addSubcommand(QueryBuildsCommand(
      environment: environment,
      configs: configs,
    ));
  }

  /// Build configurations loaded from the engine from under ci/builders.
  final Map<String, BuildConfig> configs;

  @override
  String get name => 'query';

  @override
  String get description => 'Provides information about build configurations '
                            'and tests.';

  @override
  Future<int> run() async {
    environment.stdout.write(usage);
    return 0;
  }
}

/// The 'query builds' command.
final class QueryBuildsCommand extends CommandBase {
  /// Constructs the 'query build' command.
  QueryBuildsCommand({
    required super.environment,
    required this.configs,
  });

  /// Build configurations loaded from the engine from under ci/builders.
  final Map<String, BuildConfig> configs;

  @override
  String get name => 'builds';

  @override
  String get description => 'Provides information about CI build '
                            'configurations';

  @override
  Future<int> run() async {
    // Loop through all configs, and log those that are compatible with the
    // current platform.
    final bool all = parent!.argResults![_allFlag]! as bool;
    final String? builderName = parent!.argResults![_builderFlag] as String?;
    final bool verbose = parent!.argResults![_verboseFlag] as bool;
    if (!verbose) {
      environment.stdout.writeln(
        'Add --verbose to see detailed information about each builder',
      );
      environment.stdout.writeln();
    }
    for (final String key in configs.keys) {
      if (builderName != null && key != builderName) {
        continue;
      }

      final BuildConfig config = configs[key]!;
      if (!config.canRunOn(environment.platform) && !all) {
        continue;
      }

      environment.stdout.writeln('"$key" builder:');
      for (final GlobalBuild build in config.builds) {
        if (!build.canRunOn(environment.platform) && !all) {
          continue;
        }
        environment.stdout.writeln('  "${build.name}" config');
        if (!verbose) {
          continue;
        }
        environment.stdout.writeln('    gn flags:');
        for (final String flag in build.gn) {
          environment.stdout.writeln('      $flag');
        }
        if (build.ninja.targets.isNotEmpty) {
          environment.stdout.writeln('    ninja targets:');
          for (final String target in build.ninja.targets) {
            environment.stdout.writeln('      $target');
          }
        }
      }
    }
    return 0;
  }
}
