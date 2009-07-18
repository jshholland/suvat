# Copyright (C) 2009 Josh Holland
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

"""This module performs all the calculations required in suvat.
None of the functions perform any error checking: that is up to
the caller."""


__all__ = ["calculate"]

import suvatext

def calculate(val_list):
    """Calculate values for displ, initv, endv, accel, time using equations of
    motion"""
    displ, initv, endv, accel, time = val_list
    # lots of delegating calls to suvatext
    if displ != None and initv != None and endv != None:
        accel, time = suvatext.calculate_at(displ, initv, endv)

    elif displ != None and initv != None and accel != None:
        endv, time = suvatext.calculate_vt(displ, initv, accel)

    elif displ != None and initv != None and time != None:
        endv, accel = suvatext.calculate_va(displ, initv, time)

    elif displ != None and endv != None and accel != None:
        initv, time = suvatext.calculate_ut(displ, endv, accel)

    elif displ != None and endv != None and time != None:
        initv, accel = suvatext.calculate_ua(displ, endv, time)

    elif displ != None and accel != None and time != None:
        initv, endv = suvatext.calculate_uv(displ, accel, time)

    elif initv != None and endv != None and accel != None:
        displ, time = suvatext.calculate_st(initv, endv, accel)

    elif initv != None and endv != None and time != None:
        displ, accel = suvatext.calculate_sa(initv, endv, time)

    elif initv != None and accel != None and time != None:
        displ, endv = suvatext.calculate_sv(initv, accel, time)

    elif endv != None and accel != None and time != None:
        displ, initv = suvatext.calculate_su(endv, accel, time)

    return (displ, initv, endv, accel, time)
