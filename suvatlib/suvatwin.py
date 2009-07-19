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


"""This module provides the windowing aspect of suvat."""


import pygtk
pygtk.require('2.0')
import gtk
from suvatlib import suvatcalc

try:
    from gettext import gettext as _
except ImportError:
    _ = lambda s: s


def make_dialog(title, message):
    """Helper function to create a gtk.Dialog given a title and a message"""
    dialog = gtk.Dialog(title, flags=gtk.DIALOG_MODAL)
    dialog.vbox.add(gtk.Label(message))
    dialog.add_button(_("OK"), 0)
    dialog.connect("response", lambda a, b: dialog.destroy())
    dialog.set_default_response(0)
    dialog.show_all()

class SuvatWin:
    """The main class for the program"""

    def cb_calculate(self, widget, data=None):
        """Callback for when the Calculate button is clicked"""
        number_known = 0
        raw_list = []
        for entry in [self.s_entry,
                        self.u_entry,
                        self.v_entry,
                        self.a_entry,
                        self.t_entry]:
            if entry.get_text():
                number_known += 1
            raw_list.append(entry.get_text())
            raw_list = [(elem or None) for elem in raw_list]
        if number_known < 3:
            make_dialog(_("Three variables needed!"),
                _("Three variables are needed to perform the calculation"))
            return
        if number_known > 3:
            make_dialog(_("Too many variables given!"),
                    _("More than three variables can cause contradictions."))
            return
        val_list = []
        try:
            for i in raw_list:
                if i:
                    val_list.append(float(i))
                else:
                    val_list.append(i)
        except ValueError, e:
            make_dialog(_("Invalid input!"),
                    _(str(e)))
            return

        try:
            displ, initv, endv, accel, time = suvatcalc.calculate(val_list)
        except ValueError, e:
            make_dialog(_("Cannot solve!"),
                    _(str(e)))
            return

        self.s_entry.set_text(str(displ))
        self.u_entry.set_text(str(initv))
        self.v_entry.set_text(str(endv))
        self.a_entry.set_text(str(accel))
        self.t_entry.set_text(str(time))


    def cb_reset(self, widget, data=None):
        """Callback to reset all text fields to empty"""
        for widget in (getattr(self, s + "_entry") for s in "suvat"):
            widget.set_text("")

    def __init__(self):
        self.window = gtk.Window()
        self.window.set_title(_("Suvat v0.1.1"))
        self.window.connect("destroy", lambda w: gtk.main_quit())

        self.table = gtk.Table(2, 6, True)

        self.s_entry = gtk.Entry()
        self.table.attach(self.s_entry, 1, 2, 0, 1)

        self.u_entry = gtk.Entry()
        self.table.attach(self.u_entry, 3, 4, 0, 1)

        self.v_entry = gtk.Entry()
        self.table.attach(self.v_entry, 5, 6, 0, 1)

        self.a_entry = gtk.Entry()
        self.table.attach(self.a_entry, 1, 2, 1, 2)

        self.t_entry = gtk.Entry()
        self.table.attach(self.t_entry, 3, 4, 1, 2)

        self.table.attach(gtk.Label(_("Displacement (m):")), 0, 1, 0, 1)

        self.table.attach(gtk.Label(_("Original velocity (m/s):")), 2, 3, 0, 1)

        self.table.attach(gtk.Label(_("Final velocity (m/s):")), 4, 5, 0, 1)

        self.table.attach(gtk.Label(_("Acceleration (m/s^2):")), 0, 1, 1, 2)

        self.table.attach(gtk.Label(_("Time (s):")), 2, 3, 1, 2)

        hbox = gtk.HBox()
        button = gtk.Button(_("Calculate"))
        button.connect("clicked", self.cb_calculate)
        hbox.pack_start(button)
        button = gtk.Button(_("Reset"))
        button.connect("clicked", self.cb_reset)
        hbox.pack_start(button)
        self.table.attach(hbox, 4, 6, 1, 2)

        self.window.add(self.table)
        self.window.show_all()



def main():
    """Typical PyGTK main() function"""
    SuvatWin()
    gtk.main()
