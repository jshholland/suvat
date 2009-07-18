#!/usr/bin/env python
from distutils.core import setup, Extension

setup(name = 'suvat',
      version = '0.1.1',
      description = ' '.join('A simple GUI to solve equations of motion under\
              constant acceleration'.split()),
      long_description = ' '.join('suvat uses the equations of motion with\
              constant acceleration to find the values of displacement,\
              initial velocity, final velocity, acceleration and time,\
              given three of them'.split()),
      author = 'Josh Holland',
      author_email = 'jrh@joshh.co.uk',
      packages = ['suvatlib'],
      scripts = ['suvat'],
      ext_modules = [Extension('suvatext', sources = ['suvatlib/suvatext.c'])],
      requires = ['pygtk (>= 2.0)', 'gtk'],
      license = 'GPL-3',
      url = 'https://launchpad.net/suvat')
