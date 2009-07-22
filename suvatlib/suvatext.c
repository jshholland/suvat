/* Copyright (C) 2009 Josh Holland
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warrant of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* This extension module to suvat provides the low-level implementations
 * of the calculations needed by suvat.
 *
 * Each function finds the two details given in its name (i.e. suvatext_at
 * finds acceleration and time from the other three details).
 */

/* TODO: * improve general finding of data, including special cases where
 *           time == 0
 *       * perform more error checking on data to ensure user input sensible
 */

#include <Python.h>
#include <math.h>

/* used to tell check_args which arguments are known.
 * (otherwise risk of uninitialised data.
 */
static struct {
    unsigned s_known : 1;
    unsigned u_known : 1;
    unsigned v_known : 1;
    unsigned a_known : 1;
    unsigned t_known : 1;
} known;

/* check the args to ensure that there are no contradictions
 * uses the known struct to find out which parameters are known
 * return 0 on valid args, -1 on invalid
 */
static int
check_args(double displ, double initv, double endv, double accel,
                double time) {
    int invalid = 0;        /* flag invalid args */

    /* check that things are not changing in zero time */
    if (known.t_known && time == 0) {
        /* check we are not covering distance */
        if (known.s_known && displ != 0) {
            invalid = -1;
            goto done;
        }
        /* check velocity has not changed */
        if (known.u_known && known.v_known && initv != endv) {
            invalid = -1;
            goto done;
        }
    }

    /* if we are not covering distance, do a basic check for speed */
    if (known.s_known && displ == 0) {
        if (known.u_known && initv) {
            invalid = -1;
            goto done;
        }
        if (known.v_known && endv) {
            invalid = -1;
            goto done;
        }
        if (known.u_known && known.v_known && initv == -endv) { /* we assume constant a */
            invalid = -1;
            goto done;
        }
    }

    /* check that acceleration is the right way round */
    if (known.u_known && known.v_known && initv * endv < 0) /* they have a different sign */
        if (known.a_known && (endv - initv) * accel < 0) {
            invalid = -1;
            goto done; /* both v - u and a must have the same sign */
        }

    /* check that speed is not changing if acceleration == 0 */
    if (known.u_known && known.v_known && known.a_known)
        if (accel == 0 && initv != endv) {
            invalid = -1;
            goto done;
        }

done:   if (invalid) PyErr_SetString(PyExc_ValueError, "Invalid input data");
        known.s_known = known.u_known = known.v_known = known.a_known = known.t_known = 0;
        return invalid;
}

/* the functions below do the actual calculation.
 * their name gives the two variables they calculate
 * they return (Python) 2-tuples containing the two variables they
 * have calculated and take (Python) 3-tuples with the other three
 * as arguments
 */
static PyObject *
suvatext_at(PyObject *self, PyObject *args) {
    double displ=0.0, initv=0.0, endv=0.0, accel=0.0, time=0.0;
    known.s_known = known.u_known = known.v_known = 1;
    if (!PyArg_ParseTuple(args, "ddd", &displ, &initv, &endv)
            || check_args(displ, initv, endv, accel, time))
        return NULL;

    if (initv + endv != 0) {
        /* s = t(u + v) / 2
         * t = 2s / (u + v)
         */
        time = (2 * displ) / (initv + endv);

        if (time != 0) {
            /* v = u + at
             * a = (v - u) / t
             */
            accel = (endv - initv) / time;
        } else {
            PyErr_SetString(PyExc_ValueError, "Cannot solve given that data");
            return NULL;
        }
    } else {
        /* v^2 = u^2 + 2as
         * a = (v^2 - u^2) / 2s
         */
        accel = (endv * endv - initv * initv) / (2 * displ);

        if (accel != 0) {
            /* s = u + at
             * t = (s - u) / a
             */
            time = (displ - initv) / accel;
        } else {
            PyErr_SetString(PyExc_ValueError, "Cannot solve given that data");
            return NULL;
        }
    }
    return Py_BuildValue("dd", accel, time);
}

static PyObject *
suvatext_vt(PyObject *self, PyObject *args) {
    double displ=0.0, initv=0.0, endv=0.0, accel=0.0, time=0.0;
    known.s_known = known.u_known = known.a_known = 1;
    if (!PyArg_ParseTuple(args, "ddd", &displ, &initv, &accel)
            || check_args(displ, initv, endv, accel, time))
        return NULL;

    /* v^2 = u^2 + 2as */
    endv = sqrt(fabs(initv * initv + 2.0 * accel * displ));

    /* correct sign of v */
    if (accel < 0 && endv >= initv)
        endv = -endv;
    if (accel > 0 && endv <= initv)
        endv = -endv;

    if (accel != 0) {
        /* v = u + at
         * t = (v - u) / a
         */
        time = (endv - initv) / accel;
    } else if (initv + endv != 0) {
        /* s = t(u + v) / 2
         * t = 2s / (u + v)
         */
        time = (2 * displ) / (initv + endv);
    } else {
        PyErr_SetString(PyExc_ValueError, "Cannot solve given that data");
        return NULL;
    }

    return Py_BuildValue("dd", endv, time);
}

static PyObject *
suvatext_va(PyObject *self, PyObject *args) {
    double displ=0.0, initv=0.0, endv=0.0, accel=0.0, time=0.0;
    known.s_known = known.u_known = known.t_known = 1;
    if (!PyArg_ParseTuple(args, "ddd", &displ, &initv, &time)
            || check_args(displ, initv, endv, accel, time))
        return NULL;

    if (time != 0) {
        /* s = t(u + v) / 2
         * v = 2s / t - u
         */
        endv = (2 * displ) / time - initv;

        /* v = u + at
         * a = (v - u) / t
         */
        accel = (endv - initv) / time;
    } else {
        PyErr_SetString(PyExc_ValueError, "Cannot solve given that data");
        return NULL;
    }

    return Py_BuildValue("dd", endv, accel);
}

static PyObject *
suvatext_ut(PyObject *self, PyObject *args) {
    double displ=0.0, initv=0.0, endv=0.0, accel=0.0, time=0.0;
    known.s_known = known.v_known = known.a_known = 1;
    if (!PyArg_ParseTuple(args, "ddd", &displ, &endv, &accel)
            || check_args(displ, initv, endv, accel, time))
        return NULL;

    /* v^2 = u^2 + 2as
     * u^2 = v^2 - 2as
     */
    initv = sqrt(fabs(endv * endv - 2.0 * accel * displ));

    if (accel != 0) {
        /* v = u + at
         * t = (v - u) / a
         */
        time = (endv - initv) / accel;
    } else if (initv + endv != 0) {
        /* s = t(u + v) / 2
         * t = 2s / (u + v)
         */
        time = (2 * displ) / (initv + endv);
    } else {
        PyErr_SetString(PyExc_ValueError, "Cannot solve given that data");
        return NULL;
    }

    return Py_BuildValue("dd", initv, time);
}

static PyObject *
suvatext_ua(PyObject *self, PyObject *args) {
    double displ=0.0, initv=0.0, endv=0.0, accel=0.0, time=0.0;
    known.s_known = known.v_known = known.t_known = 1;
    if (!PyArg_ParseTuple(args, "ddd", &displ, &endv, &time)
            || check_args(displ, initv, endv, accel, time))
        return NULL;

    if (time != 0) {
        /* s = t(u + v) / 2
         * u = 2s / t - v
         */
        initv = (2 * displ) / time - endv;

        /* v = u + at
         * a = (v - u) / t
         */
        accel = (endv - initv) / time;
    } else {
        PyErr_SetString(PyExc_ValueError, "Cannot solve given that data");
        return NULL;
    }

    return Py_BuildValue("dd", initv, accel);
}

static PyObject *
suvatext_uv(PyObject *self, PyObject *args) {
    double displ=0.0, initv=0.0, endv=0.0, accel=0.0, time=0.0;
    known.s_known = known.a_known = known.t_known = 1;
    if (!PyArg_ParseTuple(args, "ddd", &displ, &accel, &time)
            || check_args(displ, initv, endv, accel, time))
        return NULL;

    if (time != 0) {
        /* s = ut + .5at^2
         * u = (s - .5at^2) / t
         */
        initv = (displ - 0.5 * accel * time * time) / time;

        /* v = u + at */
        endv = initv + accel * time;
    } else {
        PyErr_SetString(PyExc_ValueError, "Cannot solve given that data");
        return NULL;
    }

    return Py_BuildValue("dd", initv, endv);
}

static PyObject *
suvatext_st(PyObject *self, PyObject *args) {
    double displ=0.0, initv=0.0, endv=0.0, accel=0.0, time=0.0;
    known.u_known = known.v_known = known.a_known = 1;
    if (!PyArg_ParseTuple(args, "ddd", &initv, &endv, &accel)
            || check_args(displ, initv, endv, accel, time))
        return NULL;

    if (accel != 0) {
        /* v^2 = u^2 + 2as
         * s = (v^2 - u^2) / 2a
         */
        displ = (endv * endv - initv * initv) / (2.0 * accel);

        if (initv + endv != 0) {
            /* s = t(u + v) / 2
             * t = 2s / (u + v)
             */
            time = (2.0 * displ) / (initv + endv);
        } else {
            /* v = u + at
             * t = (v - u) / a
             */
            time = (endv - initv) / accel;
        }
    } else {
        PyErr_SetString(PyExc_ValueError, "Cannot solve given that data");
        return NULL;
    }

    return Py_BuildValue("dd", displ, time);
}

static PyObject *
suvatext_sa(PyObject *self, PyObject *args) {
    double displ=0.0, initv=0.0, endv=0.0, accel=0.0, time=0.0;
    known.u_known = known.v_known = known.t_known = 1;
    if (!PyArg_ParseTuple(args, "ddd", &initv, &endv, &time)
            || check_args(displ, initv, endv, accel, time))
        return NULL;

    /* s = t(u + v) / 2 */
    displ = time * (initv + endv) / 2;

    if (time != 0) {
        /* v = u + at
         * a = (v - u) / t
         */
        accel = (endv - initv) / time;
    } else { /* if t = 0, s = 0 so no way we can find a */
        PyErr_SetString(PyExc_ValueError, "Cannot solve given that data");
        return NULL;
    }

    return Py_BuildValue("dd", displ, accel);
}

static PyObject *
suvatext_sv(PyObject *self, PyObject *args) {
    double displ=0.0, initv=0.0, endv=0.0, accel=0.0, time=0.0;
    known.u_known = known.a_known = known.t_known = 1;
    if (!PyArg_ParseTuple(args, "ddd", &initv, &accel, &time)
            || check_args(displ, initv, endv, accel, time))
        return NULL;

    /* v = u + at */
    endv = initv + accel * time;

    /* s = t(u + v) / 2 */
    displ = time * (initv + endv) / 2;

    return Py_BuildValue("dd", displ, endv);
}

static PyObject *
suvatext_su(PyObject *self, PyObject *args) {
    double displ=0.0, initv=0.0, endv=0.0, accel=0.0, time=0.0;
    known.v_known = known.a_known = known.t_known = 1;
    if (!PyArg_ParseTuple(args, "ddd", &endv, &accel, &time)
            || check_args(displ, initv, endv, accel, time))
        return NULL;

    /* s = vt - .5at^2 */
    displ = endv * time - 0.5 * accel * time * time;

    /* v = u + at
     * u = v - at
     */
    initv = endv - accel * time;

    return Py_BuildValue("dd", displ, initv);
}

static PyMethodDef SuvatMethods[] = {

    {"calculate_at", suvatext_at, METH_VARARGS,
        "Return the tuple (accel, time) given input displ, initv, endv."},

    {"calculate_vt", suvatext_vt, METH_VARARGS,
        "Return the tuple (endv, time) given input displ, initv, accel."},

    {"calculate_va", suvatext_va, METH_VARARGS,
        "Return the tuple (endv, accel) given input displ, initv, time."},

    {"calculate_ut", suvatext_ut, METH_VARARGS,
        "Return the tuple (initv, time) given input displ, endv, accel."},

    {"calculate_ua", suvatext_ua, METH_VARARGS,
        "Return the tuple (initv, accel) given input displ, endv, time."},

    {"calculate_uv", suvatext_uv, METH_VARARGS,
        "Return the tuple (initv, endv) given input displ, accel, time."},

    {"calculate_st", suvatext_st, METH_VARARGS,
        "Return the tuple (displ, time) given input initv, endv, accel."},

    {"calculate_sa", suvatext_sa, METH_VARARGS,
        "Return the tuple (displ, accel) given input initv, endv, time."},

    {"calculate_sv", suvatext_sv, METH_VARARGS,
        "Return the tuple (displ, endv) given input initv, accel, time."},

    {"calculate_su", suvatext_su, METH_VARARGS,
        "Return the tuple (displ, initv) given input endv, accel, time."},

    {NULL, NULL, 0, NULL} /* sentinel */

};

PyMODINIT_FUNC
initsuvatext(void) {
    (void) Py_InitModule("suvatext", SuvatMethods);
}

int
main(int argc, char *argv[]) {
    Py_SetProgramName(argv[0]);

    Py_Initialize();

    initsuvatext();
    return 0;
}
