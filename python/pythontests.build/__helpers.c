// This file contains helper functions that are automatically created from
// templates.

#include "nuitka/prelude.h"

extern PyObject *callPythonFunction( PyObject *func, PyObject **args, int count );


PyObject *CALL_FUNCTION_WITH_ARGS1(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 1; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 1 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 1; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 1 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 1 * sizeof(PyObject *));
            memcpy(python_pars + 1, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 1)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 1 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 1; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 1 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 1 * sizeof(PyObject *));
                memcpy(python_pars+1 + 1, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 1)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 1 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (1 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 1 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (1 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 1);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 1, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 1);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            1
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 1);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS2(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 2; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 2 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 2; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 2 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 2 * sizeof(PyObject *));
            memcpy(python_pars + 2, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 2)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 2 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 2; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 2 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 2 * sizeof(PyObject *));
                memcpy(python_pars+1 + 2, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 2)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 2 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (2 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 2 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (2 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 2);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 2, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 2);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            2
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 2);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS3(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 3; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 3 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 3; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 3 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 3 * sizeof(PyObject *));
            memcpy(python_pars + 3, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 3)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 3 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 3; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 3 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 3 * sizeof(PyObject *));
                memcpy(python_pars+1 + 3, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 3)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 3 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (3 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 3 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (3 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 3);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 3, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 3);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            3
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 3);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS4(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 4; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 4 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 4; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 4 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 4 * sizeof(PyObject *));
            memcpy(python_pars + 4, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 4)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 4 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 4; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 4 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 4 * sizeof(PyObject *));
                memcpy(python_pars+1 + 4, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 4)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 4 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (4 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 4 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (4 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 4);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 4, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 4);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            4
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 4);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS5(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 5; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 5 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 5; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 5 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 5 * sizeof(PyObject *));
            memcpy(python_pars + 5, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 5)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 5 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 5; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 5 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 5 * sizeof(PyObject *));
                memcpy(python_pars+1 + 5, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 5)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 5 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (5 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 5 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (5 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 5);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 5, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 5);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            5
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 5);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_METHOD_WITH_ARGS1(PyObject *source, PyObject *attr_name, PyObject **args) {
    CHECK_OBJECT(source);
    CHECK_OBJECT(attr_name);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 1; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    PyTypeObject *type = Py_TYPE(source);

    if (type->tp_getattro == PyObject_GenericGetAttr) {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely(type->tp_dict == NULL)) {
            if (unlikely(PyType_Ready(type) < 0)) {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup(type, attr_name);
        descrgetfunc func = NULL;

        if (descr != NULL)
        {
            Py_INCREF(descr);

#if PYTHON_VERSION < 300
            if (PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_HAVE_CLASS)) {
#endif
                func = Py_TYPE(descr)->tp_descr_get;

                if (func != NULL && PyDescr_IsData(descr))
                {
                    PyObject *called_object = func(descr, source, (PyObject *)type);
                    Py_DECREF(descr);

                    PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                        called_object,
                        args
                    );
                    Py_DECREF(called_object);
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if (dict != NULL)
        {
            CHECK_OBJECT(dict);

            Py_INCREF(dict);

            PyObject *called_object = PyDict_GetItem(dict, attr_name);

            if (called_object != NULL)
            {
                Py_INCREF(called_object);
                Py_XDECREF(descr);
                Py_DECREF(dict);

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF(called_object);
                return result;
            }

            Py_DECREF(dict);
        }

        if (func != NULL) {
            if (func == Nuitka_Function_Type.tp_descr_get) {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    1
                );

                Py_DECREF(descr);

                return result;
            } else {
                PyObject *called_object = func(descr, source, (PyObject *)type);
                CHECK_OBJECT(called_object);

                Py_DECREF(descr);

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF(called_object);

                return result;
            }
        }

        if (descr != NULL) {
            CHECK_OBJECT(descr);

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                descr,
                args
            );
            Py_DECREF(descr);

            return result;
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if (type == &PyInstance_Type) {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert(attr_name != const_str_plain___dict__);
        assert(attr_name != const_str_plain___class__);

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            return CALL_FUNCTION_WITH_ARGS1(called_object, args);
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            descrgetfunc descr_get = Py_TYPE(called_object)->tp_descr_get;

            if (descr_get == Nuitka_Function_Type.tp_descr_get) {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    1
                );
            } else if (descr_get != NULL) {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely(method == NULL)) {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(method, args);
                Py_DECREF(method);
                return result;
            } else {
                return CALL_FUNCTION_WITH_ARGS1(called_object, args);
            }

        } else if (unlikely(source_instance->in_class->cl_getattr == NULL)) {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        } else {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely(called_object == NULL))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                called_object,
                args
            );
            Py_DECREF(called_object);
            return result;
        }
    }
#endif
    else if (type->tp_getattro != NULL) {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely(called_object == NULL)) {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else if (type->tp_getattr != NULL) {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            (char *)Nuitka_String_AsString_Unchecked(attr_name)
        );

        if (unlikely(called_object == NULL))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked(attr_name)
        );

        return NULL;
    }
}
/* Code to register embedded modules for meta path based loading if any. */

#include "nuitka/unfreezing.h"

/* Table for lookup to find compiled or bytecode modules included in this
 * binary or module, or put along this binary as extension modules. We do
 * our own loading for each of these.
 */
extern PyObject *modulecode___main__(PyObject *);
static struct Nuitka_MetaPathBasedLoaderEntry meta_path_loader_entries[] = {
    {"__main__", modulecode___main__, 0, 0, },
    {"_asyncio", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_bisect", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_blake2", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_bz2", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_cn", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_hk", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_iso2022", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_jp", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_kr", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_tw", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_contextvars", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_crypt", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_csv", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_ctypes", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_curses", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_curses_panel", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_datetime", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_dbm", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_elementtree", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_gdbm", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_hashlib", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_heapq", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_json", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_lsprof", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_lzma", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_multibytecodec", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_multiprocessing", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_opcode", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_pickle", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_posixshmem", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_posixsubprocess", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_queue", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_random", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_sha3", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_sha512", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_socket", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_sqlite3", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_ssl", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_statistics", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_struct", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_tkinter", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_uuid", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"array", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"audioop", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"binascii", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"fcntl", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"grp", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"math", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"mmap", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"pyexpat", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"readline", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"select", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"termios", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"unicodedata", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"zlib", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"__future__", NULL, 534, 4115, NUITKA_BYTECODE_FLAG},
    {"_bootlocale", NULL, 4649, 1227, NUITKA_BYTECODE_FLAG},
    {"_collections_abc", NULL, 5876, 28725, NUITKA_BYTECODE_FLAG},
    {"_compat_pickle", NULL, 34601, 5485, NUITKA_BYTECODE_FLAG},
    {"_compression", NULL, 40086, 4130, NUITKA_BYTECODE_FLAG},
    {"_dummy_thread", NULL, 44216, 6021, NUITKA_BYTECODE_FLAG},
    {"_markupbase", NULL, 50237, 7774, NUITKA_BYTECODE_FLAG},
    {"_osx_support", NULL, 58011, 10296, NUITKA_BYTECODE_FLAG},
    {"_py_abc", NULL, 68307, 4654, NUITKA_BYTECODE_FLAG},
    {"_pyio", NULL, 72961, 74063, NUITKA_BYTECODE_FLAG},
    {"_sitebuiltins", NULL, 147024, 3465, NUITKA_BYTECODE_FLAG},
    {"_strptime", NULL, 150489, 16028, NUITKA_BYTECODE_FLAG},
    {"_sysconfigdata__linux_x86_64-linux-gnu", NULL, 166517, 20614, NUITKA_BYTECODE_FLAG},
    {"_threading_local", NULL, 187131, 6430, NUITKA_BYTECODE_FLAG},
    {"_weakrefset", NULL, 193561, 7584, NUITKA_BYTECODE_FLAG},
    {"abc", NULL, 201145, 5318, NUITKA_BYTECODE_FLAG},
    {"aifc", NULL, 206463, 25458, NUITKA_BYTECODE_FLAG},
    {"argparse", NULL, 231921, 62261, NUITKA_BYTECODE_FLAG},
    {"ast", NULL, 294182, 16665, NUITKA_BYTECODE_FLAG},
    {"asynchat", NULL, 310847, 6835, NUITKA_BYTECODE_FLAG},
    {"asyncio", NULL, 317682, 734, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"asyncio.base_events", NULL, 318416, 51004, NUITKA_BYTECODE_FLAG},
    {"asyncio.base_futures", NULL, 369420, 1707, NUITKA_BYTECODE_FLAG},
    {"asyncio.base_subprocess", NULL, 371127, 9400, NUITKA_BYTECODE_FLAG},
    {"asyncio.base_tasks", NULL, 380527, 1928, NUITKA_BYTECODE_FLAG},
    {"asyncio.constants", NULL, 382455, 565, NUITKA_BYTECODE_FLAG},
    {"asyncio.coroutines", NULL, 383020, 6638, NUITKA_BYTECODE_FLAG},
    {"asyncio.events", NULL, 389658, 28041, NUITKA_BYTECODE_FLAG},
    {"asyncio.exceptions", NULL, 417699, 2482, NUITKA_BYTECODE_FLAG},
    {"asyncio.format_helpers", NULL, 420181, 2316, NUITKA_BYTECODE_FLAG},
    {"asyncio.futures", NULL, 422497, 11177, NUITKA_BYTECODE_FLAG},
    {"asyncio.locks", NULL, 433674, 16352, NUITKA_BYTECODE_FLAG},
    {"asyncio.log", NULL, 450026, 210, NUITKA_BYTECODE_FLAG},
    {"asyncio.proactor_events", NULL, 450236, 24078, NUITKA_BYTECODE_FLAG},
    {"asyncio.protocols", NULL, 474314, 8598, NUITKA_BYTECODE_FLAG},
    {"asyncio.queues", NULL, 482912, 8365, NUITKA_BYTECODE_FLAG},
    {"asyncio.runners", NULL, 491277, 1922, NUITKA_BYTECODE_FLAG},
    {"asyncio.selector_events", NULL, 493199, 29663, NUITKA_BYTECODE_FLAG},
    {"asyncio.sslproto", NULL, 522862, 21602, NUITKA_BYTECODE_FLAG},
    {"asyncio.staggered", NULL, 544464, 4102, NUITKA_BYTECODE_FLAG},
    {"asyncio.streams", NULL, 548566, 20624, NUITKA_BYTECODE_FLAG},
    {"asyncio.subprocess", NULL, 569190, 7342, NUITKA_BYTECODE_FLAG},
    {"asyncio.tasks", NULL, 576532, 24111, NUITKA_BYTECODE_FLAG},
    {"asyncio.transports", NULL, 600643, 12243, NUITKA_BYTECODE_FLAG},
    {"asyncio.trsock", NULL, 612886, 8467, NUITKA_BYTECODE_FLAG},
    {"asyncio.unix_events", NULL, 621353, 39308, NUITKA_BYTECODE_FLAG},
    {"asyncore", NULL, 660661, 16012, NUITKA_BYTECODE_FLAG},
    {"base64", NULL, 676673, 17055, NUITKA_BYTECODE_FLAG},
    {"bdb", NULL, 693728, 24905, NUITKA_BYTECODE_FLAG},
    {"binhex", NULL, 718633, 12119, NUITKA_BYTECODE_FLAG},
    {"bisect", NULL, 730752, 2338, NUITKA_BYTECODE_FLAG},
    {"bz2", NULL, 733090, 11429, NUITKA_BYTECODE_FLAG},
    {"cProfile", NULL, 744519, 5318, NUITKA_BYTECODE_FLAG},
    {"calendar", NULL, 749837, 27048, NUITKA_BYTECODE_FLAG},
    {"cgi", NULL, 776885, 26291, NUITKA_BYTECODE_FLAG},
    {"cgitb", NULL, 803176, 10134, NUITKA_BYTECODE_FLAG},
    {"chunk", NULL, 813310, 4823, NUITKA_BYTECODE_FLAG},
    {"cmd", NULL, 818133, 12610, NUITKA_BYTECODE_FLAG},
    {"code", NULL, 830743, 9897, NUITKA_BYTECODE_FLAG},
    {"codecs", NULL, 840640, 33940, NUITKA_BYTECODE_FLAG},
    {"codeop", NULL, 874580, 6402, NUITKA_BYTECODE_FLAG},
    {"collections", NULL, 880982, 46419, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"collections.abc", NULL, 5876, 28725, NUITKA_BYTECODE_FLAG},
    {"colorsys", NULL, 927401, 3224, NUITKA_BYTECODE_FLAG},
    {"compileall", NULL, 930625, 9394, NUITKA_BYTECODE_FLAG},
    {"concurrent", NULL, 940019, 116, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"concurrent.futures", NULL, 940135, 1092, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"concurrent.futures._base", NULL, 941227, 21884, NUITKA_BYTECODE_FLAG},
    {"concurrent.futures.process", NULL, 963111, 20284, NUITKA_BYTECODE_FLAG},
    {"concurrent.futures.thread", NULL, 983395, 5802, NUITKA_BYTECODE_FLAG},
    {"configparser", NULL, 989197, 45702, NUITKA_BYTECODE_FLAG},
    {"contextlib", NULL, 1034899, 20213, NUITKA_BYTECODE_FLAG},
    {"contextvars", NULL, 1055112, 227, NUITKA_BYTECODE_FLAG},
    {"copy", NULL, 1055339, 6971, NUITKA_BYTECODE_FLAG},
    {"copyreg", NULL, 1062310, 4302, NUITKA_BYTECODE_FLAG},
    {"crypt", NULL, 1066612, 3371, NUITKA_BYTECODE_FLAG},
    {"csv", NULL, 1069983, 11894, NUITKA_BYTECODE_FLAG},
    {"ctypes", NULL, 1081877, 16356, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"ctypes._aix", NULL, 1098233, 9828, NUITKA_BYTECODE_FLAG},
    {"ctypes._endian", NULL, 1108061, 1913, NUITKA_BYTECODE_FLAG},
    {"ctypes.macholib", NULL, 1109974, 283, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"ctypes.macholib.dyld", NULL, 1110257, 4361, NUITKA_BYTECODE_FLAG},
    {"ctypes.macholib.dylib", NULL, 1114618, 1900, NUITKA_BYTECODE_FLAG},
    {"ctypes.macholib.framework", NULL, 1116518, 2180, NUITKA_BYTECODE_FLAG},
    {"ctypes.util", NULL, 1118698, 7745, NUITKA_BYTECODE_FLAG},
    {"curses", NULL, 1126443, 2124, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"curses.ascii", NULL, 1128567, 3941, NUITKA_BYTECODE_FLAG},
    {"curses.has_key", NULL, 1132508, 4539, NUITKA_BYTECODE_FLAG},
    {"curses.panel", NULL, 1137047, 207, NUITKA_BYTECODE_FLAG},
    {"curses.textpad", NULL, 1137254, 5891, NUITKA_BYTECODE_FLAG},
    {"dataclasses", NULL, 1143145, 23611, NUITKA_BYTECODE_FLAG},
    {"datetime", NULL, 1166756, 56962, NUITKA_BYTECODE_FLAG},
    {"dbm", NULL, 1223718, 4176, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"dbm.dumb", NULL, 1227894, 7757, NUITKA_BYTECODE_FLAG},
    {"dbm.gnu", NULL, 1235651, 187, NUITKA_BYTECODE_FLAG},
    {"dbm.ndbm", NULL, 1235838, 186, NUITKA_BYTECODE_FLAG},
    {"decimal", NULL, 1236024, 160719, NUITKA_BYTECODE_FLAG},
    {"difflib", NULL, 1396743, 59422, NUITKA_BYTECODE_FLAG},
    {"dis", NULL, 1456165, 15786, NUITKA_BYTECODE_FLAG},
    {"distutils", NULL, 1471951, 368, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"distutils.archive_util", NULL, 1472319, 6523, NUITKA_BYTECODE_FLAG},
    {"distutils.bcppcompiler", NULL, 1478842, 6507, NUITKA_BYTECODE_FLAG},
    {"distutils.ccompiler", NULL, 1485349, 33281, NUITKA_BYTECODE_FLAG},
    {"distutils.cmd", NULL, 1518630, 13932, NUITKA_BYTECODE_FLAG},
    {"distutils.command", NULL, 1532562, 527, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"distutils.command.bdist", NULL, 1533089, 3650, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_dumb", NULL, 1536739, 3576, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_rpm", NULL, 1540315, 12412, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_wininst", NULL, 1552727, 8473, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build", NULL, 1561200, 3865, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_clib", NULL, 1565065, 4798, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_ext", NULL, 1569863, 16121, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_py", NULL, 1585984, 10464, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_scripts", NULL, 1596448, 4308, NUITKA_BYTECODE_FLAG},
    {"distutils.command.check", NULL, 1600756, 4869, NUITKA_BYTECODE_FLAG},
    {"distutils.command.clean", NULL, 1605625, 2084, NUITKA_BYTECODE_FLAG},
    {"distutils.command.config", NULL, 1607709, 10211, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install", NULL, 1617920, 13556, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_data", NULL, 1631476, 2273, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_egg_info", NULL, 1633749, 2980, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_headers", NULL, 1636729, 1692, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_lib", NULL, 1638421, 5097, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_scripts", NULL, 1643518, 2125, NUITKA_BYTECODE_FLAG},
    {"distutils.command.register", NULL, 1645643, 8439, NUITKA_BYTECODE_FLAG},
    {"distutils.command.sdist", NULL, 1654082, 14500, NUITKA_BYTECODE_FLAG},
    {"distutils.command.upload", NULL, 1668582, 4923, NUITKA_BYTECODE_FLAG},
    {"distutils.config", NULL, 1673505, 3493, NUITKA_BYTECODE_FLAG},
    {"distutils.core", NULL, 1676998, 6598, NUITKA_BYTECODE_FLAG},
    {"distutils.cygwinccompiler", NULL, 1683596, 8596, NUITKA_BYTECODE_FLAG},
    {"distutils.debug", NULL, 1692192, 178, NUITKA_BYTECODE_FLAG},
    {"distutils.dep_util", NULL, 1692370, 2698, NUITKA_BYTECODE_FLAG},
    {"distutils.dir_util", NULL, 1695068, 5807, NUITKA_BYTECODE_FLAG},
    {"distutils.dist", NULL, 1700875, 34476, NUITKA_BYTECODE_FLAG},
    {"distutils.errors", NULL, 1735351, 5234, NUITKA_BYTECODE_FLAG},
    {"distutils.extension", NULL, 1740585, 6907, NUITKA_BYTECODE_FLAG},
    {"distutils.fancy_getopt", NULL, 1747492, 10640, NUITKA_BYTECODE_FLAG},
    {"distutils.file_util", NULL, 1758132, 5917, NUITKA_BYTECODE_FLAG},
    {"distutils.filelist", NULL, 1764049, 9851, NUITKA_BYTECODE_FLAG},
    {"distutils.log", NULL, 1773900, 2299, NUITKA_BYTECODE_FLAG},
    {"distutils.msvccompiler", NULL, 1776199, 14707, NUITKA_BYTECODE_FLAG},
    {"distutils.spawn", NULL, 1790906, 5090, NUITKA_BYTECODE_FLAG},
    {"distutils.sysconfig", NULL, 1795996, 12085, NUITKA_BYTECODE_FLAG},
    {"distutils.text_file", NULL, 1808081, 8423, NUITKA_BYTECODE_FLAG},
    {"distutils.unixccompiler", NULL, 1816504, 6602, NUITKA_BYTECODE_FLAG},
    {"distutils.util", NULL, 1823106, 15530, NUITKA_BYTECODE_FLAG},
    {"distutils.version", NULL, 1838636, 7295, NUITKA_BYTECODE_FLAG},
    {"distutils.versionpredicate", NULL, 1845931, 5119, NUITKA_BYTECODE_FLAG},
    {"doctest", NULL, 1851050, 75958, NUITKA_BYTECODE_FLAG},
    {"dummy_threading", NULL, 1927008, 1094, NUITKA_BYTECODE_FLAG},
    {"email", NULL, 1928102, 1675, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"email._encoded_words", NULL, 1929777, 5670, NUITKA_BYTECODE_FLAG},
    {"email._header_value_parser", NULL, 1935447, 79917, NUITKA_BYTECODE_FLAG},
    {"email._parseaddr", NULL, 2015364, 12438, NUITKA_BYTECODE_FLAG},
    {"email._policybase", NULL, 2027802, 14794, NUITKA_BYTECODE_FLAG},
    {"email.base64mime", NULL, 2042596, 3219, NUITKA_BYTECODE_FLAG},
    {"email.charset", NULL, 2045815, 11443, NUITKA_BYTECODE_FLAG},
    {"email.contentmanager", NULL, 2057258, 7345, NUITKA_BYTECODE_FLAG},
    {"email.encoders", NULL, 2064603, 1596, NUITKA_BYTECODE_FLAG},
    {"email.errors", NULL, 2066199, 5889, NUITKA_BYTECODE_FLAG},
    {"email.feedparser", NULL, 2072088, 10626, NUITKA_BYTECODE_FLAG},
    {"email.generator", NULL, 2082714, 12466, NUITKA_BYTECODE_FLAG},
    {"email.header", NULL, 2095180, 16423, NUITKA_BYTECODE_FLAG},
    {"email.headerregistry", NULL, 2111603, 22028, NUITKA_BYTECODE_FLAG},
    {"email.iterators", NULL, 2133631, 1904, NUITKA_BYTECODE_FLAG},
    {"email.message", NULL, 2135535, 37862, NUITKA_BYTECODE_FLAG},
    {"email.mime", NULL, 2173397, 116, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"email.mime.application", NULL, 2173513, 1443, NUITKA_BYTECODE_FLAG},
    {"email.mime.audio", NULL, 2174956, 2608, NUITKA_BYTECODE_FLAG},
    {"email.mime.base", NULL, 2177564, 1025, NUITKA_BYTECODE_FLAG},
    {"email.mime.image", NULL, 2178589, 1888, NUITKA_BYTECODE_FLAG},
    {"email.mime.message", NULL, 2180477, 1266, NUITKA_BYTECODE_FLAG},
    {"email.mime.multipart", NULL, 2181743, 1486, NUITKA_BYTECODE_FLAG},
    {"email.mime.nonmultipart", NULL, 2183229, 748, NUITKA_BYTECODE_FLAG},
    {"email.mime.text", NULL, 2183977, 1295, NUITKA_BYTECODE_FLAG},
    {"email.parser", NULL, 2185272, 5706, NUITKA_BYTECODE_FLAG},
    {"email.policy", NULL, 2190978, 9642, NUITKA_BYTECODE_FLAG},
    {"email.quoprimime", NULL, 2200620, 7662, NUITKA_BYTECODE_FLAG},
    {"email.utils", NULL, 2208282, 9526, NUITKA_BYTECODE_FLAG},
    {"encodings", NULL, 2217808, 3887, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"encodings.aliases", NULL, 2221695, 6314, NUITKA_BYTECODE_FLAG},
    {"encodings.ascii", NULL, 2228009, 1865, NUITKA_BYTECODE_FLAG},
    {"encodings.base64_codec", NULL, 2229874, 2383, NUITKA_BYTECODE_FLAG},
    {"encodings.big5", NULL, 2232257, 1393, NUITKA_BYTECODE_FLAG},
    {"encodings.big5hkscs", NULL, 2233650, 1403, NUITKA_BYTECODE_FLAG},
    {"encodings.bz2_codec", NULL, 2235053, 3274, NUITKA_BYTECODE_FLAG},
    {"encodings.charmap", NULL, 2238327, 2875, NUITKA_BYTECODE_FLAG},
    {"encodings.cp037", NULL, 2241202, 2406, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1006", NULL, 2243608, 2482, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1026", NULL, 2246090, 2410, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1125", NULL, 2248500, 8113, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1140", NULL, 2256613, 2396, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1250", NULL, 2259009, 2433, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1251", NULL, 2261442, 2430, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1252", NULL, 2263872, 2433, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1253", NULL, 2266305, 2446, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1254", NULL, 2268751, 2435, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1255", NULL, 2271186, 2454, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1256", NULL, 2273640, 2432, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1257", NULL, 2276072, 2440, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1258", NULL, 2278512, 2438, NUITKA_BYTECODE_FLAG},
    {"encodings.cp273", NULL, 2280950, 2392, NUITKA_BYTECODE_FLAG},
    {"encodings.cp424", NULL, 2283342, 2436, NUITKA_BYTECODE_FLAG},
    {"encodings.cp437", NULL, 2285778, 7830, NUITKA_BYTECODE_FLAG},
    {"encodings.cp500", NULL, 2293608, 2406, NUITKA_BYTECODE_FLAG},
    {"encodings.cp720", NULL, 2296014, 2503, NUITKA_BYTECODE_FLAG},
    {"encodings.cp737", NULL, 2298517, 8152, NUITKA_BYTECODE_FLAG},
    {"encodings.cp775", NULL, 2306669, 7860, NUITKA_BYTECODE_FLAG},
    {"encodings.cp850", NULL, 2314529, 7491, NUITKA_BYTECODE_FLAG},
    {"encodings.cp852", NULL, 2322020, 7868, NUITKA_BYTECODE_FLAG},
    {"encodings.cp855", NULL, 2329888, 8121, NUITKA_BYTECODE_FLAG},
    {"encodings.cp856", NULL, 2338009, 2468, NUITKA_BYTECODE_FLAG},
    {"encodings.cp857", NULL, 2340477, 7471, NUITKA_BYTECODE_FLAG},
    {"encodings.cp858", NULL, 2347948, 7461, NUITKA_BYTECODE_FLAG},
    {"encodings.cp860", NULL, 2355409, 7809, NUITKA_BYTECODE_FLAG},
    {"encodings.cp861", NULL, 2363218, 7824, NUITKA_BYTECODE_FLAG},
    {"encodings.cp862", NULL, 2371042, 8013, NUITKA_BYTECODE_FLAG},
    {"encodings.cp863", NULL, 2379055, 7824, NUITKA_BYTECODE_FLAG},
    {"encodings.cp864", NULL, 2386879, 7968, NUITKA_BYTECODE_FLAG},
    {"encodings.cp865", NULL, 2394847, 7824, NUITKA_BYTECODE_FLAG},
    {"encodings.cp866", NULL, 2402671, 8157, NUITKA_BYTECODE_FLAG},
    {"encodings.cp869", NULL, 2410828, 7848, NUITKA_BYTECODE_FLAG},
    {"encodings.cp874", NULL, 2418676, 2534, NUITKA_BYTECODE_FLAG},
    {"encodings.cp875", NULL, 2421210, 2403, NUITKA_BYTECODE_FLAG},
    {"encodings.cp932", NULL, 2423613, 1395, NUITKA_BYTECODE_FLAG},
    {"encodings.cp949", NULL, 2425008, 1395, NUITKA_BYTECODE_FLAG},
    {"encodings.cp950", NULL, 2426403, 1395, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_jis_2004", NULL, 2427798, 1409, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_jisx0213", NULL, 2429207, 1409, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_jp", NULL, 2430616, 1397, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_kr", NULL, 2432013, 1397, NUITKA_BYTECODE_FLAG},
    {"encodings.gb18030", NULL, 2433410, 1399, NUITKA_BYTECODE_FLAG},
    {"encodings.gb2312", NULL, 2434809, 1397, NUITKA_BYTECODE_FLAG},
    {"encodings.gbk", NULL, 2436206, 1391, NUITKA_BYTECODE_FLAG},
    {"encodings.hex_codec", NULL, 2437597, 2370, NUITKA_BYTECODE_FLAG},
    {"encodings.hp_roman8", NULL, 2439967, 2607, NUITKA_BYTECODE_FLAG},
    {"encodings.hz", NULL, 2442574, 1389, NUITKA_BYTECODE_FLAG},
    {"encodings.idna", NULL, 2443963, 5601, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp", NULL, 2449564, 1410, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_1", NULL, 2450974, 1414, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_2", NULL, 2452388, 1414, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_2004", NULL, 2453802, 1420, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_3", NULL, 2455222, 1414, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_ext", NULL, 2456636, 1418, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_kr", NULL, 2458054, 1410, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_1", NULL, 2459464, 2405, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_10", NULL, 2461869, 2410, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_11", NULL, 2464279, 2504, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_13", NULL, 2466783, 2413, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_14", NULL, 2469196, 2431, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_15", NULL, 2471627, 2410, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_16", NULL, 2474037, 2412, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_2", NULL, 2476449, 2405, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_3", NULL, 2478854, 2412, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_4", NULL, 2481266, 2405, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_5", NULL, 2483671, 2406, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_6", NULL, 2486077, 2450, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_7", NULL, 2488527, 2413, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_8", NULL, 2490940, 2444, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_9", NULL, 2493384, 2405, NUITKA_BYTECODE_FLAG},
    {"encodings.johab", NULL, 2495789, 1395, NUITKA_BYTECODE_FLAG},
    {"encodings.koi8_r", NULL, 2497184, 2457, NUITKA_BYTECODE_FLAG},
    {"encodings.koi8_t", NULL, 2499641, 2368, NUITKA_BYTECODE_FLAG},
    {"encodings.koi8_u", NULL, 2502009, 2443, NUITKA_BYTECODE_FLAG},
    {"encodings.kz1048", NULL, 2504452, 2420, NUITKA_BYTECODE_FLAG},
    {"encodings.latin_1", NULL, 2506872, 1877, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_arabic", NULL, 2508749, 7724, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_centeuro", NULL, 2516473, 2444, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_croatian", NULL, 2518917, 2452, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_cyrillic", NULL, 2521369, 2442, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_farsi", NULL, 2523811, 2386, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_greek", NULL, 2526197, 2426, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_iceland", NULL, 2528623, 2445, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_latin2", NULL, 2531068, 2586, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_roman", NULL, 2533654, 2443, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_romanian", NULL, 2536097, 2453, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_turkish", NULL, 2538550, 2446, NUITKA_BYTECODE_FLAG},
    {"encodings.palmos", NULL, 2540996, 2433, NUITKA_BYTECODE_FLAG},
    {"encodings.ptcp154", NULL, 2543429, 2527, NUITKA_BYTECODE_FLAG},
    {"encodings.punycode", NULL, 2545956, 6299, NUITKA_BYTECODE_FLAG},
    {"encodings.quopri_codec", NULL, 2552255, 2399, NUITKA_BYTECODE_FLAG},
    {"encodings.raw_unicode_escape", NULL, 2554654, 1746, NUITKA_BYTECODE_FLAG},
    {"encodings.rot_13", NULL, 2556400, 2985, NUITKA_BYTECODE_FLAG},
    {"encodings.shift_jis", NULL, 2559385, 1403, NUITKA_BYTECODE_FLAG},
    {"encodings.shift_jis_2004", NULL, 2560788, 1413, NUITKA_BYTECODE_FLAG},
    {"encodings.shift_jisx0213", NULL, 2562201, 1413, NUITKA_BYTECODE_FLAG},
    {"encodings.tis_620", NULL, 2563614, 2495, NUITKA_BYTECODE_FLAG},
    {"encodings.undefined", NULL, 2566109, 2079, NUITKA_BYTECODE_FLAG},
    {"encodings.unicode_escape", NULL, 2568188, 1726, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_16", NULL, 2569914, 4856, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_16_be", NULL, 2574770, 1634, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_16_le", NULL, 2576404, 1634, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_32", NULL, 2578038, 4749, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_32_be", NULL, 2582787, 1527, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_32_le", NULL, 2584314, 1527, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_7", NULL, 2585841, 1555, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_8", NULL, 2587396, 1614, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_8_sig", NULL, 2589010, 4530, NUITKA_BYTECODE_FLAG},
    {"encodings.uu_codec", NULL, 2593540, 3233, NUITKA_BYTECODE_FLAG},
    {"encodings.zlib_codec", NULL, 2596773, 3093, NUITKA_BYTECODE_FLAG},
    {"enum", NULL, 2599866, 24490, NUITKA_BYTECODE_FLAG},
    {"filecmp", NULL, 2624356, 8411, NUITKA_BYTECODE_FLAG},
    {"fileinput", NULL, 2632767, 13357, NUITKA_BYTECODE_FLAG},
    {"fnmatch", NULL, 2646124, 3316, NUITKA_BYTECODE_FLAG},
    {"formatter", NULL, 2649440, 17529, NUITKA_BYTECODE_FLAG},
    {"fractions", NULL, 2666969, 18723, NUITKA_BYTECODE_FLAG},
    {"ftplib", NULL, 2685692, 27833, NUITKA_BYTECODE_FLAG},
    {"functools", NULL, 2713525, 27885, NUITKA_BYTECODE_FLAG},
    {"genericpath", NULL, 2741410, 3985, NUITKA_BYTECODE_FLAG},
    {"getopt", NULL, 2745395, 6255, NUITKA_BYTECODE_FLAG},
    {"getpass", NULL, 2751650, 4162, NUITKA_BYTECODE_FLAG},
    {"gettext", NULL, 2755812, 17867, NUITKA_BYTECODE_FLAG},
    {"glob", NULL, 2773679, 4327, NUITKA_BYTECODE_FLAG},
    {"gzip", NULL, 2778006, 18175, NUITKA_BYTECODE_FLAG},
    {"hashlib", NULL, 2796181, 6711, NUITKA_BYTECODE_FLAG},
    {"heapq", NULL, 2802892, 14054, NUITKA_BYTECODE_FLAG},
    {"hmac", NULL, 2816946, 6372, NUITKA_BYTECODE_FLAG},
    {"html", NULL, 2823318, 3602, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"html.entities", NULL, 2826920, 50513, NUITKA_BYTECODE_FLAG},
    {"html.parser", NULL, 2877433, 11182, NUITKA_BYTECODE_FLAG},
    {"http", NULL, 2888615, 6048, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"http.client", NULL, 2894663, 34670, NUITKA_BYTECODE_FLAG},
    {"http.cookiejar", NULL, 2929333, 53626, NUITKA_BYTECODE_FLAG},
    {"http.cookies", NULL, 2982959, 15252, NUITKA_BYTECODE_FLAG},
    {"http.server", NULL, 2998211, 34376, NUITKA_BYTECODE_FLAG},
    {"imaplib", NULL, 3032587, 41326, NUITKA_BYTECODE_FLAG},
    {"imghdr", NULL, 3073913, 4102, NUITKA_BYTECODE_FLAG},
    {"imp", NULL, 3078015, 9793, NUITKA_BYTECODE_FLAG},
    {"importlib", NULL, 3087808, 3742, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"importlib._bootstrap", NULL, 3091550, 28589, NUITKA_BYTECODE_FLAG},
    {"importlib._bootstrap_external", NULL, 3120139, 43694, NUITKA_BYTECODE_FLAG},
    {"importlib.abc", NULL, 3163833, 13557, NUITKA_BYTECODE_FLAG},
    {"importlib.machinery", NULL, 3177390, 946, NUITKA_BYTECODE_FLAG},
    {"importlib.metadata", NULL, 3178336, 20824, NUITKA_BYTECODE_FLAG},
    {"importlib.resources", NULL, 3199160, 6464, NUITKA_BYTECODE_FLAG},
    {"importlib.util", NULL, 3205624, 9276, NUITKA_BYTECODE_FLAG},
    {"inspect", NULL, 3214900, 80367, NUITKA_BYTECODE_FLAG},
    {"io", NULL, 3295267, 3438, NUITKA_BYTECODE_FLAG},
    {"ipaddress", NULL, 3298705, 59552, NUITKA_BYTECODE_FLAG},
    {"json", NULL, 3358257, 12592, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"json.decoder", NULL, 3370849, 9828, NUITKA_BYTECODE_FLAG},
    {"json.encoder", NULL, 3380677, 11155, NUITKA_BYTECODE_FLAG},
    {"json.scanner", NULL, 3391832, 1935, NUITKA_BYTECODE_FLAG},
    {"json.tool", NULL, 3393767, 1880, NUITKA_BYTECODE_FLAG},
    {"keyword", NULL, 3395647, 982, NUITKA_BYTECODE_FLAG},
    {"lib2to3", NULL, 3396629, 113, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.btm_matcher", NULL, 3396742, 4861, NUITKA_BYTECODE_FLAG},
    {"lib2to3.btm_utils", NULL, 3401603, 6134, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixer_base", NULL, 3407737, 6237, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixer_util", NULL, 3413974, 12182, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes", NULL, 3426156, 119, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.fixes.fix_apply", NULL, 3426275, 1657, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_asserts", NULL, 3427932, 1256, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_basestring", NULL, 3429188, 640, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_buffer", NULL, 3429828, 785, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_dict", NULL, 3430613, 3312, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_except", NULL, 3433925, 2795, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_exec", NULL, 3436720, 1126, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_execfile", NULL, 3437846, 1674, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_exitfunc", NULL, 3439520, 2283, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_filter", NULL, 3441803, 2421, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_funcattrs", NULL, 3444224, 953, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_future", NULL, 3445177, 761, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_getcwdu", NULL, 3445938, 765, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_has_key", NULL, 3446703, 2916, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_idioms", NULL, 3449619, 3898, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_import", NULL, 3453517, 2764, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_imports", NULL, 3456281, 4372, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_imports2", NULL, 3460653, 523, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_input", NULL, 3461176, 927, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_intern", NULL, 3462103, 1111, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_isinstance", NULL, 3463214, 1528, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_itertools", NULL, 3464742, 1527, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_itertools_imports", NULL, 3466269, 1555, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_long", NULL, 3467824, 682, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_map", NULL, 3468506, 3074, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_metaclass", NULL, 3471580, 5324, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_methodattrs", NULL, 3476904, 915, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_ne", NULL, 3477819, 788, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_next", NULL, 3478607, 3060, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_nonzero", NULL, 3481667, 900, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_numliterals", NULL, 3482567, 1000, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_operator", NULL, 3483567, 4197, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_paren", NULL, 3487764, 1367, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_print", NULL, 3489131, 2320, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_raise", NULL, 3491451, 2230, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_raw_input", NULL, 3493681, 772, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_reduce", NULL, 3494453, 1105, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_reload", NULL, 3495558, 1123, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_renames", NULL, 3496681, 1984, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_repr", NULL, 3498665, 822, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_set_literal", NULL, 3499487, 1660, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_standarderror", NULL, 3501147, 697, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_sys_exc", NULL, 3501844, 1388, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_throw", NULL, 3503232, 1785, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_tuple_params", NULL, 3505017, 4568, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_types", NULL, 3509585, 1814, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_unicode", NULL, 3511399, 1526, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_urllib", NULL, 3512925, 5976, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_ws_comma", NULL, 3518901, 1096, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_xrange", NULL, 3519997, 2506, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_xreadlines", NULL, 3522503, 1100, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_zip", NULL, 3523603, 1560, NUITKA_BYTECODE_FLAG},
    {"lib2to3.main", NULL, 3525163, 8592, NUITKA_BYTECODE_FLAG},
    {"lib2to3.patcomp", NULL, 3533755, 5623, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2", NULL, 3539378, 149, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.pgen2.driver", NULL, 3539527, 5125, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.grammar", NULL, 3544652, 5649, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.literals", NULL, 3550301, 1538, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.parse", NULL, 3551839, 6482, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.pgen", NULL, 3558321, 9760, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.token", NULL, 3568081, 1871, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.tokenize", NULL, 3569952, 15256, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pygram", NULL, 3585208, 1244, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pytree", NULL, 3586452, 24326, NUITKA_BYTECODE_FLAG},
    {"lib2to3.refactor", NULL, 3610778, 20424, NUITKA_BYTECODE_FLAG},
    {"linecache", NULL, 3631202, 3851, NUITKA_BYTECODE_FLAG},
    {"locale", NULL, 3635053, 34673, NUITKA_BYTECODE_FLAG},
    {"logging", NULL, 3669726, 64855, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"logging.config", NULL, 3734581, 23208, NUITKA_BYTECODE_FLAG},
    {"logging.handlers", NULL, 3757789, 43140, NUITKA_BYTECODE_FLAG},
    {"lzma", NULL, 3800929, 12002, NUITKA_BYTECODE_FLAG},
    {"mailbox", NULL, 3812931, 60248, NUITKA_BYTECODE_FLAG},
    {"mailcap", NULL, 3873179, 6460, NUITKA_BYTECODE_FLAG},
    {"mimetypes", NULL, 3879639, 16015, NUITKA_BYTECODE_FLAG},
    {"modulefinder", NULL, 3895654, 16099, NUITKA_BYTECODE_FLAG},
    {"multiprocessing", NULL, 3911753, 646, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"multiprocessing.connection", NULL, 3912399, 25218, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.context", NULL, 3937617, 13002, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.dummy", NULL, 3950619, 3887, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"multiprocessing.dummy.connection", NULL, 3954506, 2532, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.forkserver", NULL, 3957038, 8343, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.heap", NULL, 3965381, 7621, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.managers", NULL, 3973002, 41530, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.pool", NULL, 4014532, 25009, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.popen_fork", NULL, 4039541, 2628, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.popen_forkserver", NULL, 4042169, 2401, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.popen_spawn_posix", NULL, 4044570, 2192, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.process", NULL, 4046762, 10985, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.queues", NULL, 4057747, 9538, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.reduction", NULL, 4067285, 8187, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.resource_sharer", NULL, 4075472, 5240, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.resource_tracker", NULL, 4080712, 5169, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.shared_memory", NULL, 4085881, 14300, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.sharedctypes", NULL, 4100181, 7033, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.spawn", NULL, 4107214, 6680, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.synchronize", NULL, 4113894, 11274, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.util", NULL, 4125168, 11470, NUITKA_BYTECODE_FLAG},
    {"netrc", NULL, 4136638, 3761, NUITKA_BYTECODE_FLAG},
    {"nntplib", NULL, 4140399, 33958, NUITKA_BYTECODE_FLAG},
    {"ntpath", NULL, 4174357, 14641, NUITKA_BYTECODE_FLAG},
    {"nturl2path", NULL, 4188998, 1585, NUITKA_BYTECODE_FLAG},
    {"numbers", NULL, 4190583, 12186, NUITKA_BYTECODE_FLAG},
    {"opcode", NULL, 4202769, 5404, NUITKA_BYTECODE_FLAG},
    {"operator", NULL, 4208173, 13675, NUITKA_BYTECODE_FLAG},
    {"optparse", NULL, 4221848, 48041, NUITKA_BYTECODE_FLAG},
    {"os", NULL, 4269889, 31381, NUITKA_BYTECODE_FLAG},
    {"pathlib", NULL, 4301270, 43536, NUITKA_BYTECODE_FLAG},
    {"pdb", NULL, 4344806, 47176, NUITKA_BYTECODE_FLAG},
    {"pickle", NULL, 4391982, 46863, NUITKA_BYTECODE_FLAG},
    {"pickletools", NULL, 4438845, 67188, NUITKA_BYTECODE_FLAG},
    {"pipes", NULL, 4506033, 7779, NUITKA_BYTECODE_FLAG},
    {"pkgutil", NULL, 4513812, 16293, NUITKA_BYTECODE_FLAG},
    {"platform", NULL, 4530105, 24224, NUITKA_BYTECODE_FLAG},
    {"plistlib", NULL, 4554329, 26637, NUITKA_BYTECODE_FLAG},
    {"poplib", NULL, 4580966, 13443, NUITKA_BYTECODE_FLAG},
    {"posixpath", NULL, 4594409, 10412, NUITKA_BYTECODE_FLAG},
    {"pprint", NULL, 4604821, 16265, NUITKA_BYTECODE_FLAG},
    {"profile", NULL, 4621086, 14598, NUITKA_BYTECODE_FLAG},
    {"pstats", NULL, 4635684, 22037, NUITKA_BYTECODE_FLAG},
    {"pty", NULL, 4657721, 3939, NUITKA_BYTECODE_FLAG},
    {"py_compile", NULL, 4661660, 7378, NUITKA_BYTECODE_FLAG},
    {"pyclbr", NULL, 4669038, 10435, NUITKA_BYTECODE_FLAG},
    {"pydoc", NULL, 4679473, 84071, NUITKA_BYTECODE_FLAG},
    {"pydoc_data", NULL, 4763544, 116, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"pydoc_data.topics", NULL, 4763660, 420491, NUITKA_BYTECODE_FLAG},
    {"queue", NULL, 5184151, 10610, NUITKA_BYTECODE_FLAG},
    {"quopri", NULL, 5194761, 5732, NUITKA_BYTECODE_FLAG},
    {"random", NULL, 5200493, 20092, NUITKA_BYTECODE_FLAG},
    {"re", NULL, 5220585, 14406, NUITKA_BYTECODE_FLAG},
    {"reprlib", NULL, 5234991, 5287, NUITKA_BYTECODE_FLAG},
    {"rlcompleter", NULL, 5240278, 5739, NUITKA_BYTECODE_FLAG},
    {"runpy", NULL, 5246017, 8165, NUITKA_BYTECODE_FLAG},
    {"sched", NULL, 5254182, 6516, NUITKA_BYTECODE_FLAG},
    {"secrets", NULL, 5260698, 2174, NUITKA_BYTECODE_FLAG},
    {"selectors", NULL, 5262872, 16919, NUITKA_BYTECODE_FLAG},
    {"shelve", NULL, 5279791, 9474, NUITKA_BYTECODE_FLAG},
    {"shlex", NULL, 5289265, 7520, NUITKA_BYTECODE_FLAG},
    {"shutil", NULL, 5296785, 36553, NUITKA_BYTECODE_FLAG},
    {"signal", NULL, 5333338, 2827, NUITKA_BYTECODE_FLAG},
    {"site", NULL, 5336165, 13256, NUITKA_BYTECODE_FLAG},
    {"smtpd", NULL, 5349421, 26447, NUITKA_BYTECODE_FLAG},
    {"smtplib", NULL, 5375868, 35297, NUITKA_BYTECODE_FLAG},
    {"sndhdr", NULL, 5411165, 6973, NUITKA_BYTECODE_FLAG},
    {"socket", NULL, 5418138, 27771, NUITKA_BYTECODE_FLAG},
    {"socketserver", NULL, 5445909, 23991, NUITKA_BYTECODE_FLAG},
    {"sqlite3", NULL, 5469900, 144, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"sqlite3.dbapi2", NULL, 5470044, 2488, NUITKA_BYTECODE_FLAG},
    {"sqlite3.dump", NULL, 5472532, 1913, NUITKA_BYTECODE_FLAG},
    {"sre_compile", NULL, 5474445, 15126, NUITKA_BYTECODE_FLAG},
    {"sre_constants", NULL, 5489571, 6343, NUITKA_BYTECODE_FLAG},
    {"sre_parse", NULL, 5495914, 21631, NUITKA_BYTECODE_FLAG},
    {"ssl", NULL, 5517545, 44580, NUITKA_BYTECODE_FLAG},
    {"stat", NULL, 5562125, 4356, NUITKA_BYTECODE_FLAG},
    {"statistics", NULL, 5566481, 33637, NUITKA_BYTECODE_FLAG},
    {"string", NULL, 5600118, 7284, NUITKA_BYTECODE_FLAG},
    {"stringprep", NULL, 5607402, 11001, NUITKA_BYTECODE_FLAG},
    {"struct", NULL, 5618403, 314, NUITKA_BYTECODE_FLAG},
    {"subprocess", NULL, 5618717, 41924, NUITKA_BYTECODE_FLAG},
    {"sunau", NULL, 5660641, 17064, NUITKA_BYTECODE_FLAG},
    {"symbol", NULL, 5677705, 2388, NUITKA_BYTECODE_FLAG},
    {"symtable", NULL, 5680093, 11060, NUITKA_BYTECODE_FLAG},
    {"sysconfig", NULL, 5691153, 15604, NUITKA_BYTECODE_FLAG},
    {"tabnanny", NULL, 5706757, 7014, NUITKA_BYTECODE_FLAG},
    {"tarfile", NULL, 5713771, 62564, NUITKA_BYTECODE_FLAG},
    {"telnetlib", NULL, 5776335, 18221, NUITKA_BYTECODE_FLAG},
    {"tempfile", NULL, 5794556, 23439, NUITKA_BYTECODE_FLAG},
    {"textwrap", NULL, 5817995, 13503, NUITKA_BYTECODE_FLAG},
    {"this", NULL, 5831498, 1245, NUITKA_BYTECODE_FLAG},
    {"threading", NULL, 5832743, 39960, NUITKA_BYTECODE_FLAG},
    {"timeit", NULL, 5872703, 11761, NUITKA_BYTECODE_FLAG},
    {"tkinter", NULL, 5884464, 175986, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"tkinter.colorchooser", NULL, 6060450, 1109, NUITKA_BYTECODE_FLAG},
    {"tkinter.commondialog", NULL, 6061559, 1106, NUITKA_BYTECODE_FLAG},
    {"tkinter.constants", NULL, 6062665, 1638, NUITKA_BYTECODE_FLAG},
    {"tkinter.dialog", NULL, 6064303, 1456, NUITKA_BYTECODE_FLAG},
    {"tkinter.dnd", NULL, 6065759, 11246, NUITKA_BYTECODE_FLAG},
    {"tkinter.filedialog", NULL, 6077005, 12212, NUITKA_BYTECODE_FLAG},
    {"tkinter.font", NULL, 6089217, 6181, NUITKA_BYTECODE_FLAG},
    {"tkinter.messagebox", NULL, 6095398, 2975, NUITKA_BYTECODE_FLAG},
    {"tkinter.scrolledtext", NULL, 6098373, 2161, NUITKA_BYTECODE_FLAG},
    {"tkinter.simpledialog", NULL, 6100534, 10600, NUITKA_BYTECODE_FLAG},
    {"tkinter.tix", NULL, 6111134, 79502, NUITKA_BYTECODE_FLAG},
    {"tkinter.ttk", NULL, 6190636, 56741, NUITKA_BYTECODE_FLAG},
    {"token", NULL, 6247377, 2469, NUITKA_BYTECODE_FLAG},
    {"tokenize", NULL, 6249846, 17144, NUITKA_BYTECODE_FLAG},
    {"trace", NULL, 6266990, 20010, NUITKA_BYTECODE_FLAG},
    {"traceback", NULL, 6287000, 19873, NUITKA_BYTECODE_FLAG},
    {"tracemalloc", NULL, 6306873, 17347, NUITKA_BYTECODE_FLAG},
    {"tty", NULL, 6324220, 1060, NUITKA_BYTECODE_FLAG},
    {"turtle", NULL, 6325280, 129924, NUITKA_BYTECODE_FLAG},
    {"types", NULL, 6455204, 9161, NUITKA_BYTECODE_FLAG},
    {"typing", NULL, 6464365, 62404, NUITKA_BYTECODE_FLAG},
    {"unittest", NULL, 6526769, 3110, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"unittest.async_case", NULL, 6529879, 4124, NUITKA_BYTECODE_FLAG},
    {"unittest.case", NULL, 6534003, 50468, NUITKA_BYTECODE_FLAG},
    {"unittest.loader", NULL, 6584471, 14520, NUITKA_BYTECODE_FLAG},
    {"unittest.main", NULL, 6598991, 7518, NUITKA_BYTECODE_FLAG},
    {"unittest.mock", NULL, 6606509, 77350, NUITKA_BYTECODE_FLAG},
    {"unittest.result", NULL, 6683859, 7275, NUITKA_BYTECODE_FLAG},
    {"unittest.runner", NULL, 6691134, 7000, NUITKA_BYTECODE_FLAG},
    {"unittest.signals", NULL, 6698134, 2204, NUITKA_BYTECODE_FLAG},
    {"unittest.suite", NULL, 6700338, 9919, NUITKA_BYTECODE_FLAG},
    {"unittest.util", NULL, 6710257, 4415, NUITKA_BYTECODE_FLAG},
    {"urllib", NULL, 6714672, 112, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"urllib.error", NULL, 6714784, 2793, NUITKA_BYTECODE_FLAG},
    {"urllib.parse", NULL, 6717577, 33916, NUITKA_BYTECODE_FLAG},
    {"urllib.request", NULL, 6751493, 72515, NUITKA_BYTECODE_FLAG},
    {"urllib.response", NULL, 6824008, 3273, NUITKA_BYTECODE_FLAG},
    {"urllib.robotparser", NULL, 6827281, 7311, NUITKA_BYTECODE_FLAG},
    {"uu", NULL, 6834592, 3589, NUITKA_BYTECODE_FLAG},
    {"uuid", NULL, 6838181, 23650, NUITKA_BYTECODE_FLAG},
    {"venv", NULL, 6861831, 14415, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"warnings", NULL, 6876246, 13636, NUITKA_BYTECODE_FLAG},
    {"wave", NULL, 6889882, 18133, NUITKA_BYTECODE_FLAG},
    {"weakref", NULL, 6908015, 19502, NUITKA_BYTECODE_FLAG},
    {"webbrowser", NULL, 6927517, 17082, NUITKA_BYTECODE_FLAG},
    {"wsgiref", NULL, 6944599, 708, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"wsgiref.handlers", NULL, 6945307, 16430, NUITKA_BYTECODE_FLAG},
    {"wsgiref.headers", NULL, 6961737, 7740, NUITKA_BYTECODE_FLAG},
    {"wsgiref.simple_server", NULL, 6969477, 5245, NUITKA_BYTECODE_FLAG},
    {"wsgiref.util", NULL, 6974722, 5396, NUITKA_BYTECODE_FLAG},
    {"wsgiref.validate", NULL, 6980118, 14739, NUITKA_BYTECODE_FLAG},
    {"xdrlib", NULL, 6994857, 8205, NUITKA_BYTECODE_FLAG},
    {"xml", NULL, 7003062, 676, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.dom", NULL, 7003738, 5514, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.dom.NodeFilter", NULL, 7009252, 951, NUITKA_BYTECODE_FLAG},
    {"xml.dom.domreg", NULL, 7010203, 2834, NUITKA_BYTECODE_FLAG},
    {"xml.dom.expatbuilder", NULL, 7013037, 27325, NUITKA_BYTECODE_FLAG},
    {"xml.dom.minicompat", NULL, 7040362, 2726, NUITKA_BYTECODE_FLAG},
    {"xml.dom.minidom", NULL, 7043088, 55358, NUITKA_BYTECODE_FLAG},
    {"xml.dom.pulldom", NULL, 7098446, 10675, NUITKA_BYTECODE_FLAG},
    {"xml.dom.xmlbuilder", NULL, 7109121, 12478, NUITKA_BYTECODE_FLAG},
    {"xml.etree", NULL, 7121599, 115, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.etree.ElementInclude", NULL, 7121714, 1563, NUITKA_BYTECODE_FLAG},
    {"xml.etree.ElementPath", NULL, 7123277, 8418, NUITKA_BYTECODE_FLAG},
    {"xml.etree.ElementTree", NULL, 7131695, 55596, NUITKA_BYTECODE_FLAG},
    {"xml.etree.cElementTree", NULL, 7187291, 157, NUITKA_BYTECODE_FLAG},
    {"xml.parsers", NULL, 7187448, 289, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.parsers.expat", NULL, 7187737, 318, NUITKA_BYTECODE_FLAG},
    {"xml.sax", NULL, 7188055, 3203, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.sax._exceptions", NULL, 7191258, 5428, NUITKA_BYTECODE_FLAG},
    {"xml.sax.expatreader", NULL, 7196686, 12479, NUITKA_BYTECODE_FLAG},
    {"xml.sax.handler", NULL, 7209165, 12406, NUITKA_BYTECODE_FLAG},
    {"xml.sax.saxutils", NULL, 7221571, 12903, NUITKA_BYTECODE_FLAG},
    {"xml.sax.xmlreader", NULL, 7234474, 16828, NUITKA_BYTECODE_FLAG},
    {"xmlrpc", NULL, 7251302, 112, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xmlrpc.client", NULL, 7251414, 34523, NUITKA_BYTECODE_FLAG},
    {"xmlrpc.server", NULL, 7285937, 29392, NUITKA_BYTECODE_FLAG},
    {"zipapp", NULL, 7315329, 5838, NUITKA_BYTECODE_FLAG},
    {"zipfile", NULL, 7321167, 58452, NUITKA_BYTECODE_FLAG},
    {NULL, NULL, 0, 0, 0}
};

void setupMetaPathBasedLoader(void) {
    static bool init_done = false;

    if (init_done == false) {
        registerMetaPathBasedUnfreezer(meta_path_loader_entries);
        init_done = true;
    }
}
