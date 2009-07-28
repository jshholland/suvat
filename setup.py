#!/usr/bin/env python
from distutils.core import setup, Extension
from distutils.command.build import build as _build
import os

class build(_build):
    def run(self):
        _build.run(self)
        os.system("desktop-file-install data/suvat.desktop")


setup(cmdclass = {'build': build},
      name = 'suvat',
      version = '0.2',
      description = 'A simple GUI to solve equations of motion under '
              'constant acceleration',
      long_description = 'suvat uses the equations of motion with '
              'constant acceleration to find the values of displacement, '
              'initial velocity, final velocity, acceleration and time, '
              'given three of them',
      author = 'Josh Holland',
      author_email = 'jrh@joshh.co.uk',
      packages = ['suvatlib'],
      scripts = ['suvat'],
      ext_modules = [Extension('suvatext', sources = ['suvatlib/suvatext.c'])],
      requires = ['pygtk (>= 2.0)', 'gtk'],
      license = 'GPL-3',
      data_files = [('/usr/share/man/man1', ['doc/suvat.1.gz'])],
      url = 'https://launchpad.net/suvat')
