#!/bin/env python3

import json
import math
import statistics

code = [
    '#pragma once',
    '// Generated by calibration.py do not edit',
    '#include <array>', '',
    'namespace esphome {',
    'namespace mq9_circut {',
    'namespace calibration {',
    '\tstruct precalculated { float m; float b;};',
]

with open('./calibration.json') as f:
    calibraion = json.load(f)
    datasets = [
        {
            'name': x['name'],
            'points': [{'index': y['x'], 'x': y['value'][0], 'y': y['value'][1]} for y in x['data']]
        }
        for x in calibraion['datasetColl']
    ]

    code.append('\tstatic constexpr std::array<precalculated,{}> setpoints{{'.format(
        len(datasets)))
    names = []
    for element in datasets:
        first = min(element['points'], key=lambda e: e['index'])
        last = max(element['points'], key=lambda e: e['index'])
        m = (math.log10(last['y']) - math.log10(first['y'])) / \
            (math.log10(last['x']) - math.log10(first['x']))
        b = statistics.mean([math.log10(
            point['y']) - (m * math.log10(point['x'])) for point in element['points']])
        code.append('\t\tprecalculated{{{:4f}f,{:4f}f}},'.format(m, b))
        names.append(element['name'].upper())
    code.append('\t};')

code.append('\tenum gas_type {')
for name in names:
    code.append('\t\t{},'.format(name))

code.append('\t};')
code.append('}')
code.append('}')
code.append('}')


with open('./calibration.h', 'w') as f:
    f.write('\n'.join(code))
