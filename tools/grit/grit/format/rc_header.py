# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

'''Item formatters for RC headers.
'''

from __future__ import print_function


def Format(root, lang='en', output_dir='.'):
  yield '''\
// This file is automatically generated by GRIT. Do not edit.

#pragma once
'''
  # Check for emit nodes under the rc_header. If any emit node
  # is present, we assume it means the GRD file wants to override
  # the default header, with no includes.
  default_includes = ['#include <atlres.h>', '']
  emit_lines = []
  for output_node in root.GetOutputFiles():
    if output_node.GetType() == 'rc_header':
      for child in output_node.children:
        if child.name == 'emit' and child.attrs['emit_type'] == 'prepend':
          emit_lines.append(child.GetCdata())
  for line in emit_lines or default_includes:
    yield line + '\n'
  if root.IsWhitelistSupportEnabled():
    yield '#include "ui/base/resource/whitelist.h"\n'
  for line in FormatDefines(root):
    yield line


def FormatDefines(root):
  '''Yields #define SYMBOL 1234 lines.

  Args:
    root: A GritNode.
  '''
  tids = root.GetIdMap()
  rc_header_format = '#define {0} {1}\n'
  if root.IsWhitelistSupportEnabled():
    rc_header_format = '#define {0} (::ui::WhitelistedResource<{1}>(), {1})\n'
  for item in root.ActiveDescendants():
    with item:
      for tid in item.GetTextualIds():
        yield rc_header_format.format(tid, tids[tid])
