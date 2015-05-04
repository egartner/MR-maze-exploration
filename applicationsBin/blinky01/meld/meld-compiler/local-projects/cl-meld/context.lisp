
(in-package :cl-meld)

(defparameter *file* nil "Compiled file path.")
(defparameter *ast* nil "Abstract Syntax Tree.")
(defparameter *code* nil "Virtual Machine instructions.")
(defparameter *code-rules* nil "Virtual Machine instructions for each rule.")

(define-symbol-macro *definitions* (definitions *ast*))
(define-symbol-macro *node-definitions* *definitions*)
(define-symbol-macro *clauses* (clauses *ast*))
(define-symbol-macro *axioms* (axioms *ast*))
(define-symbol-macro *const-axioms* (const-axioms *ast*))
(define-symbol-macro *nodes* (nodes *ast*))
(define-symbol-macro *externs* (externs *ast*))
(define-symbol-macro *functions* (functions *ast*))
(define-symbol-macro *priorities* (priorities *ast*))
(define-symbol-macro *consts* (consts *ast*))
(define-symbol-macro *processes* (processes *code*))
(define-symbol-macro *consts-code* (consts *code*))
(define-symbol-macro *function-code* (functions *code*))
(define-symbol-macro *exported-predicates* (exported-predicates *ast*))
(define-symbol-macro *imported-predicates* (imported-predicates *ast*))

(defun set-abstract-syntax-tree (ast) (setf *ast* ast))