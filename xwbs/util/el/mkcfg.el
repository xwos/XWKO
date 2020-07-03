#! /bin/sh
":"; exec emacs --quick --script "$0" -- "$@" # -*- mode: emacs-lisp; lexical-binding: t; -*-
;; @file
;; @brief Emacs-script to generate XuanWuOS configurations and makefile
;; @author
;; + 隐星魂 (Roy.Sun) <https://xwos.tech>
;; @copyright
;; + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
;; > This Source Code Form is subject to the terms of the Mozilla Public
;; > License, v. 2.0 (the "MPL"). If a copy of the MPL was not distributed
;; > with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
;; >
;; > Alternatively, the contents of this file may be used under the
;; > terms of the GNU General Public License version 2 (the "GPL"), in
;; > which case the provisions of the GPL are applicable instead of the
;; > above. If you wish to allow the use of your version of this file
;; > only under the terms of the GPL and not to allow others to use your
;; > version of this file under the MPL, indicate your decision by
;; > deleting the provisions above and replace them with the notice and
;; > other provisions required by the GPL. If you do not delete the
;; > provisions above, a recipient may use your version of this file
;; > under either the MPL or the GPL.

(defvar elscript (expand-file-name (nth 2 command-line-args)) "script name")
(defvar elpath (file-name-directory elscript))
(push elpath load-path)
(require 'errno)

;;;;;;;; ;;;;;;;; ;;;;;;;; config ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defconst cfg-log-lv 1)

;;;;;;;; ;;;;;;;; ;;;;;;;; log function ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defconst DBG "[DBG]")
(defconst DBG-LV 0)
(defconst INF "[INF]")
(defconst INF-LV 1)
(defconst WRN "[WRN]")
(defconst WRN-LV 2)
(defconst ERR "[ERR]")
(defconst ERR-LV 3)

(defun logd (fmtstr &rest arglist)
  (if (<= cfg-log-lv DBG-LV)
      (princ (apply 'format (concat DBG " " fmtstr "\n") arglist))))
(defun logi (fmtstr &rest arglist)
  (if (<= cfg-log-lv INF-LV)
      (princ (apply 'format (concat INF " " fmtstr "\n") arglist))))
(defun logw (fmtstr &rest arglist)
  (if (<= cfg-log-lv WRN-LV)
      (princ (apply 'format (concat WRN " " fmtstr "\n") arglist))))
(defun loge (fmtstr &rest arglist)
  (if (<= cfg-log-lv ERR-LV)
      (princ (apply 'format (concat ERR " " fmtstr "\n") arglist))))
(defmacro logtag (tag)
  (list 'car (list 'read-from-string (list 'concat "log" (list 'format "%s" tag)))))

(defun log (tag fmtstr &rest arglist)
  (apply (logtag tag) fmtstr arglist))

;;;;;;;; ;;;;;;;; ;;;;;;;; arguments ;;;;;;;; ;;;;;;;; ;;;;;;;;
(setq debug-on-error t)
(setq make-backup-files nil)
(defvar argc (length argv) "Count of arguments")
(defvar topdir nil "Path of TOP directory")
(defvar cfgdir nil "Path of configurations directory")
(defvar wkspc nil "Path of workspace directory")
(defvar XuanWuOS-cfg ".cfg" "Name of XuanWuOS.cfg")
(logd "argv:%s" argv)

(let ((options-done nil))
  (pop argv)  ; Remove the -- separator
  (while argv
    (let ((option (pop argv)))
      ;; (logd "option:%s" option)
      (cond
       (options-done (setq XuanWuOS-cfg option))

       ;; options after "--"
       ((string= option "--") (setq options-done t))

       ;; --cfgdir foo
       ;; -c foo
       ;; --cfgdir=foo
       ((string= option "--cfgdir") (setq cfgdir (pop argv)))
       ((string= option "-c") (setq cfgdir (pop argv)))
       ((string-match "\\`--cfgdir=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq cfgdir (match-string 1 option)))

       ;; --topdir foo
       ;; -t foo
       ;; --topdir=foo
       ((string= option "--topdir") (setq topdir (pop argv)))
       ((string= option "-t") (setq topdir (pop argv)))
       ((string-match "\\`--topdir=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq topdir (match-string 1 option)))

       ;; --wkspc foo
       ;; -w foo
       ;; --wkspc=foo
       ((string= option "--wkspc") (setq wkspc (pop argv)))
       ((string= option "-w") (setq wkspc (pop argv)))
       ((string-match "\\`--wkspc=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq wkspc (match-string 1 option)))

       ;; Unknown option
       ((string-prefix-p "--" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))

       ((string-prefix-p "-" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))

       ;; default
       (t (setq XuanWuOS-cfg option)))))

  (unless (> (length cfgdir) 0)
  (loge "Missing argument for --cfgdir!")
  (kill-emacs EINVAL))

  (unless (> (length topdir) 0)
  (loge "Missing argument for --topdir!")
  (kill-emacs EINVAL))

  (unless (> (length wkspc) 0)
  (loge "Missing argument for --wkspc!")
  (kill-emacs EINVAL)))

;;;;;;;; ;;;;;;;; ;;;;;;;; Get infomation ;;;;;;;; ;;;;;;;; ;;;;;;;;
;; Get top directory
(setq XuanWuOS-path (expand-file-name "." topdir))
(logi "Build Path:%s" (expand-file-name "."))
(logi "XuanWuOS Path:%s" XuanWuOS-path)
(logi "XuanWuOS Workspace:%s" wkspc)
(logi "XuanWuOS.cfg:%s" XuanWuOS-cfg)
(setq XuanWuOS-cfg-buffer (find-file-noselect (concat wkspc "/" XuanWuOS-cfg)))
(setq xwos-build-mk-buffer (find-file-noselect (concat wkspc "/makefile")))
(setq xwos-autogen-header-buffer (find-file-noselect (concat wkspc "/autogen.h")))
(setq xwos-envrc-buffer (find-file-noselect (concat wkspc "/env.rc")))

(defvar XuanWuOS-cfg-h (concat cfgdir "/" "XuanWuOS.h") "Path of cfg/XuanWuOS.h")
(defvar arch-cfg-h (concat cfgdir "/" "arch.h") "Path of cfg/arch.h")
(defvar cpu-cfg-h (concat cfgdir "/" "cpu.h") "Path of cfg/cpu.h")
(defvar soc-cfg-h (concat cfgdir "/" "soc.h") "Path of cfg/soc.h")
(defvar pp-cfg-h (concat cfgdir "/" "perpheral.h") "Path of cfg/perpheral.h")
(defvar brd-cfg-h (concat cfgdir "/" "board.h") "Path of cfg/board.h")
(defvar xwkn-cfg-h (concat cfgdir "/" "xwos.h") "Path of cfg/xwos.h")
(defvar xwmd-cfg-h (concat cfgdir "/" "xwmd.h") "Path of cfg/xwmd.h")
(defvar xwem-cfg-h (concat cfgdir "/" "xwem.h") "Path of cfg/xwem.h")
(defvar xwam-cfg-h (concat cfgdir "/" "xwam.h") "Path of cfg/xwam.h")
(setq XuanWuOS-cfg-h-buffer (find-file-noselect XuanWuOS-cfg-h))
(setq arch-cfg-h-buffer (find-file-noselect arch-cfg-h))
(setq cpu-cfg-h-buffer (find-file-noselect cpu-cfg-h))
(setq soc-cfg-h-buffer (find-file-noselect soc-cfg-h))
(setq pp-cfg-h-buffer (find-file-noselect pp-cfg-h))
(setq brd-cfg-h-buffer (find-file-noselect brd-cfg-h))
(setq xwkn-cfg-h-buffer (find-file-noselect xwkn-cfg-h))
(setq xwmd-cfg-h-buffer (find-file-noselect xwmd-cfg-h))
(setq xwem-cfg-h-buffer (find-file-noselect xwem-cfg-h))
(setq xwam-cfg-h-buffer (find-file-noselect xwam-cfg-h))

;; Get base Info
(set-buffer XuanWuOS-cfg-h-buffer)
(set-buffer-multibyte nil)

(logi "host:%s" (symbol-name system-type))

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_ARCH\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-arch (match-string 2))
(logi "ARCH:%s" XuanWuOS-cfg-arch)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_SUBARCH\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-subarch (match-string 2))
(logi "Sub-ARCH:%s" XuanWuOS-cfg-subarch)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_COMPILER\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-compiler (match-string 2))
(logi "compiler:%s" XuanWuOS-cfg-compiler)

(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+XuanWuOS_CFG_MK_RULE[ \t]+\\(.+\\)" nil t))
  (if (null rc)
    (setq XuanWuOS-cfg-mk-rule "")
    (setq XuanWuOS-cfg-mk-rule (match-string 1))))
(logi "XuanWuOS-cfg-mk-rule:%s" XuanWuOS-cfg-mk-rule)

(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+XuanWuOS_CFG_ELF_MK[ \t]+\\(.+\\)" nil t))
  (if (null rc)
    (setq XuanWuOS-cfg-elf-mk "")
    (setq XuanWuOS-cfg-elf-mk (match-string 1))))
(logi "XuanWuOS-cfg-elf-makefile:%s" XuanWuOS-cfg-elf-mk)

(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+XuanWuOS_CFG_XWMO_MK[ \t]+\\(.+\\)" nil t))
  (if (null rc)
    (setq XuanWuOS-cfg-xwmo-mk "")
    (setq XuanWuOS-cfg-xwmo-mk (match-string 1))))
(logi "XuanWuOS-cfg-xwmo-makefile:%s" XuanWuOS-cfg-xwmo-mk)

(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+XuanWuOS_CFG_LDSCRIPT[ \t]+\\(.+\\)" nil t))
  (if (null rc)
    (setq XuanWuOS-cfg-lds "")
    (setq XuanWuOS-cfg-lds (match-string 1))))
(logi "XuanWuOS-cfg-lds:%s" XuanWuOS-cfg-lds)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_CPU\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-cpu (match-string 2))
(logi "CPU:%s" XuanWuOS-cfg-cpu)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_SOC\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-soc (match-string 2))
(logi "SOC:%s" XuanWuOS-cfg-soc)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_PERPHERAL\\)[\t ]+\\([10]\\)" nil t)
(setq XuanWuOS-cfg-perpheral
      (cond
       ((string= (match-string 2) "1") "y")
       (t "n")))
(logi "perpheral:%s" XuanWuOS-cfg-perpheral)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_BOARD\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-brd (match-string 2))
(logi "board:%s" XuanWuOS-cfg-brd)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_CORE\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-core (match-string 2))
(logi "core:%s" XuanWuOS-cfg-core)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_XWMD\\)[\t ]+\\([10]\\)" nil t)
(setq XuanWuOS-cfg-xwmd
      (cond
       ((string= (match-string 2) "1") "y")
       (t "n")))
(logi "middleware:%s" XuanWuOS-cfg-xwmd)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_XWEM\\)[\t ]+\\([10]\\)" nil t)
(setq XuanWuOS-cfg-xwem
      (cond
       ((string= (match-string 2) "1") "y")
       (t "n")))
(logi "external:%s" XuanWuOS-cfg-xwem)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_XWAM\\)[\t ]+\\([10]\\)" nil t)
(setq XuanWuOS-cfg-xwam
      (cond
       ((string= (match-string 2) "1") "y")
       (t "n")))
(logi "external:%s" XuanWuOS-cfg-xwam)

;; Get directories Info
(setq xwos-kr-dir (concat "xwos"))
(logi "xwos-kr-dir:%s" xwos-kr-dir)
(setq xwos-md-dir (concat "xwmd"))
(logi "xwos-md-dir:%s" xwos-md-dir)
(setq xwos-em-dir (concat "xwem"))
(logi "xwos-em-dir:%s" xwos-em-dir)
(setq xwos-am-dir (concat "xwam"))
(logi "xwos-am-dir:%s" xwos-am-dir)
(setq xwos-arch-dir (concat "xwcd/soc/" XuanWuOS-cfg-arch "/"
                            XuanWuOS-cfg-subarch "/" XuanWuOS-cfg-compiler))
(logi "xwos-arch-dir:%s" xwos-arch-dir)
(setq xwos-cpu-dir (concat xwos-arch-dir "/" XuanWuOS-cfg-cpu))
(logi "xwos-cpu-dir:%s" xwos-cpu-dir)
(setq xwos-soc-dir (concat xwos-cpu-dir "/" XuanWuOS-cfg-soc))
(logi "xwos-soc-dir:%s" xwos-soc-dir)
(setq xwos-pp-dir (concat "xwcd/perpheral"))
(logi "xwos-pp-dir:%s" xwos-pp-dir)
(setq xwos-brd-dir (concat "xwbd/" XuanWuOS-cfg-brd))
(logi "xwos-brd-dir:%s" xwos-brd-dir)
(setq xwos-bdl-dir (concat xwos-brd-dir "/bdl"))
(logi "xwos-bdl-dir:%s" xwos-bdl-dir)
(setq xwos-bm-dir (concat xwos-brd-dir "/bm"))
(logi "xwos-bm-dir:%s" xwos-bm-dir)
(setq xwos-wkspc-dir (if (file-name-absolute-p wkspc)
                         wkspc
                         (concat xwos-brd-dir "/" wkspc)))
(logi "xwos-wkspc-dir:%s" xwos-wkspc-dir)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate XuanWuOS.cfg ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer XuanWuOS-cfg-buffer)
(set-buffer-multibyte nil)
(erase-buffer)
(insert "## ******** ******** ******** ******** basic info ******** ******** ******** ******** ##\n")
(insert (concat "XuanWuOS_CFG_HOSTOS := " (symbol-name system-type) "\n"))
(insert (concat "XuanWuOS_CFG_ARCH := " XuanWuOS-cfg-arch "\n"))
(insert (concat "XuanWuOS_CFG_SUBARCH := " XuanWuOS-cfg-subarch "\n"))
(insert (concat "XuanWuOS_CFG_COMPILER := " XuanWuOS-cfg-compiler "\n"))
(insert (concat "XuanWuOS_CFG_LDSCRIPT := " XuanWuOS-cfg-lds "\n"))
(insert (concat "XuanWuOS_CFG_CPU := " XuanWuOS-cfg-cpu "\n"))
(insert (concat "XuanWuOS_CFG_SOC := " XuanWuOS-cfg-soc "\n"))
(insert (concat "XuanWuOS_CFG_PERPHERAL := " XuanWuOS-cfg-perpheral "\n"))
(insert (concat "XuanWuOS_CFG_BRD := " XuanWuOS-cfg-brd "\n"))
(insert (concat "XuanWuOS_CFG_CORE := " XuanWuOS-cfg-core "\n"))
(insert (concat "XuanWuOS_CFG_XWMD := " XuanWuOS-cfg-xwmd "\n"))
(insert (concat "XuanWuOS_CFG_XWEM := " XuanWuOS-cfg-xwem "\n"))
(insert (concat "XuanWuOS_CFG_XWAM := " XuanWuOS-cfg-xwam "\n"))
(insert (concat "XuanWuOS_CFG_MK_RULE := " XuanWuOS-cfg-mk-rule "\n"))
(insert (concat "XuanWuOS_CFG_ELF_MK := " XuanWuOS-cfg-elf-mk "\n"))
(insert (concat "XuanWuOS_CFG_XWMO_MK := " XuanWuOS-cfg-xwmo-mk "\n"))
(insert "## ******** ******** ******** ******** build Script info ******** ******** ******** ******** ##\n")
(insert (concat "XWBS_UTIL_MK_XWMO := " "xwbs/util/mk/xwmo.mk\n"))
(insert "## ******** ******** ******** ******** directory info ******** ******** ******** ******** ##\n")
(insert (concat "XWOS_PATH := " XuanWuOS-path "\n"))
(insert (concat "XWOS_KN_DIR := " xwos-kr-dir "\n"))
(insert (concat "XWOS_MD_DIR := " xwos-md-dir "\n"))
(insert (concat "XWOS_EM_DIR := " xwos-em-dir "\n"))
(insert (concat "XWOS_AM_DIR := " xwos-am-dir "\n"))
(insert (concat "XWOS_ARCH_DIR := " xwos-arch-dir "\n"))
(insert (concat "XWOS_CPU_DIR := " xwos-cpu-dir "\n"))
(insert (concat "XWOS_SOC_DIR := " xwos-soc-dir "\n"))
(insert (concat "XWOS_PP_DIR := " xwos-pp-dir "\n"))
(insert (concat "XWOS_BRD_DIR := " xwos-brd-dir "\n"))
(insert (concat "XWOS_BDL_DIR := " xwos-bdl-dir "\n"))
(insert (concat "XWOS_BM_DIR := " xwos-bm-dir "\n"))
(insert (concat "XWOS_WKSPC_DIR := " xwos-wkspc-dir "\n"))
(insert "## ******** ******** ******** ******** ARCH ******** ******** ******** ******** ##\n")
(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer arch-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer arch-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** CPU ******** ******** ******** ******** ##\n")
(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer cpu-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer cpu-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** SOC ******** ******** ******** ******** ##\n")
(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer soc-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer soc-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** Perpheral ******** ******** ******** ******** ##\n")
(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer pp-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer pp-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** Board ******** ******** ******** ******** ##\n")
(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer brd-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer brd-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** XWOS Kernel ******** ******** ******** ******** ##\n")
(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer xwkn-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer xwkn-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** XWOS Middleware ******** ******** ******** ******** ##\n")
(if (string= XuanWuOS-cfg-xwmd "y")
    (let (item cfg (loopflg t) (iterpoint 1))
      (set-buffer xwmd-cfg-h-buffer)
      (while loopflg
        (goto-char iterpoint)
        (setq iterpoint (re-search-forward
                         "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
        (if (null iterpoint)
            (setq iterpoint (point-max)))
        (if (< iterpoint (point-max))
            (progn
              (setq item (match-string 1))
              (setq cfg (if (string= (match-string 2) "1") "y" "n"))
              (set-buffer XuanWuOS-cfg-buffer)
              (insert (concat item " := " cfg "\n"))
              (set-buffer xwmd-cfg-h-buffer))
          (setq loopflg nil)))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** External Module ******** ******** ******** ******** ##\n")
(if (string= XuanWuOS-cfg-xwem "y")
    (let (item cfg (loopflg t) (iterpoint 1))
      (set-buffer xwem-cfg-h-buffer)
      (while loopflg
        (goto-char iterpoint)
        (setq iterpoint (re-search-forward
                         "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
        (if (null iterpoint)
            (setq iterpoint (point-max)))
        (if (< iterpoint (point-max))
            (progn
              (setq item (match-string 1))
              (setq cfg (if (string= (match-string 2) "1") "y" "n"))
              (set-buffer XuanWuOS-cfg-buffer)
              (insert (concat item " := " cfg "\n"))
              (set-buffer xwem-cfg-h-buffer))
          (setq loopflg nil)))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** APP Module ******** ******** ******** ******** ##\n")
(if (string= XuanWuOS-cfg-xwam "y")
    (let (item cfg (loopflg t) (iterpoint 1))
      (set-buffer xwam-cfg-h-buffer)
      (while loopflg
        (goto-char iterpoint)
        (setq iterpoint (re-search-forward
                         "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
        (if (null iterpoint)
            (setq iterpoint (point-max)))
        (if (< iterpoint (point-max))
            (progn
              (setq item (match-string 1))
              (setq cfg (if (string= (match-string 2) "1") "y" "n"))
              (set-buffer XuanWuOS-cfg-buffer)
              (insert (concat item " := " cfg "\n"))
              (set-buffer xwam-cfg-h-buffer))
          (setq loopflg nil)))))
(set-buffer XuanWuOS-cfg-buffer)

;; save file
(save-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate header ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer xwos-autogen-header-buffer)
(set-buffer-multibyte nil)
(erase-buffer)
(insert (concat
"/* Don't edit this file, because it is generated by xwbs/mkcfg.el */

#ifndef __wkspc_autogen_h__
#define __wkspc_autogen_h__

#define XuanWuOS_CFG_ARCH__" XuanWuOS-cfg-arch " 1
#define XuanWuOS_CFG_SUBARCH__" XuanWuOS-cfg-subarch " 1
#define XuanWuOS_CFG_COMPILER__" XuanWuOS-cfg-compiler " 1
#define XuanWuOS_CFG_CPU__" XuanWuOS-cfg-cpu " 1
#define XuanWuOS_CFG_SOC__" XuanWuOS-cfg-soc " 1
#define XuanWuOS_CFG_BOARD__" XuanWuOS-cfg-brd " 1
#define XuanWuOS_CFG_CORE__" XuanWuOS-cfg-core " 1

#endif /* wkspc/autogen.h */
"))
(save-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate build makefile ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer xwos-build-mk-buffer)
(set-buffer-multibyte nil)
(erase-buffer)
(insert
"#! /bin/make -f
# @file
# @brief building makefile
# @note
# This file is generated by mkcfg.el
#

include XuanWuOS.cfg
TARGET ?= XuanWuOS

MAKE_ARGS = $(strip -C $(XWOS_PATH) -f xwbs/$(XuanWuOS_CFG_ELF_MK) \\
                    XWOS_PATH=$(XWOS_PATH) XWOS_WKSPC_DIR=$(XWOS_WKSPC_DIR))

all:
\t$(MAKE) $(MAKE_ARGS) all

clean:
\t@$(MAKE) $(MAKE_ARGS) clean

distclean:
\t@$(MAKE) $(MAKE_ARGS) distclean

.DEFAULT:
\t$(MAKE) $(MAKE_ARGS) $@

")
(save-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate env.rc ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer xwos-envrc-buffer)
(set-buffer-multibyte nil)
(erase-buffer)
(insert
"# @file
# @brief environment script
# @note
# This file is generated by mkcfg.el
#
")

(insert "## ******** ******** ******** ******** basic info ******** ******** ******** ******** ##\n")
(insert (concat "export XuanWuOS_CFG_HOSTOS=" (symbol-name system-type) "\n"))
(insert (concat "export XuanWuOS_CFG_ARCH=" XuanWuOS-cfg-arch "\n"))
(insert (concat "export XuanWuOS_CFG_SUBARCH=" XuanWuOS-cfg-subarch "\n"))
(insert (concat "export XuanWuOS_CFG_COMPILER=" XuanWuOS-cfg-compiler "\n"))
(insert (concat "export XuanWuOS_CFG_LDSCRIPT=" XuanWuOS-cfg-lds "\n"))
(insert (concat "export XuanWuOS_CFG_CPU=" XuanWuOS-cfg-cpu "\n"))
(insert (concat "export XuanWuOS_CFG_SOC=" XuanWuOS-cfg-soc "\n"))
(insert (concat "export XuanWuOS_CFG_PERPHERAL=" XuanWuOS-cfg-perpheral "\n"))
(insert (concat "export XuanWuOS_CFG_BRD=" XuanWuOS-cfg-brd "\n"))
(insert (concat "export XuanWuOS_CFG_CORE=" XuanWuOS-cfg-core "\n"))
(insert (concat "export XuanWuOS_CFG_XWMD=" XuanWuOS-cfg-xwmd "\n"))
(insert (concat "export XuanWuOS_CFG_XWEM=" XuanWuOS-cfg-xwem "\n"))
(insert (concat "export XuanWuOS_CFG_XWAM=" XuanWuOS-cfg-xwam "\n"))
(insert (concat "export XuanWuOS_CFG_MK_RULE=" XuanWuOS-cfg-mk-rule "\n"))
(insert (concat "export XuanWuOS_CFG_ELF_MK=" XuanWuOS-cfg-elf-mk "\n"))
(insert (concat "export XuanWuOS_CFG_XWMO_MK=" XuanWuOS-cfg-xwmo-mk "\n"))
(insert "## ******** ******** ******** ******** directory info ******** ******** ******** ******** ##\n")
(insert (concat "export XWOS_PATH=" XuanWuOS-path "\n"))
(insert (concat "export XWOS_KN_DIR=" xwos-kr-dir "\n"))
(insert (concat "export XWOS_MD_DIR=" xwos-md-dir "\n"))
(insert (concat "export XWOS_EM_DIR=" xwos-em-dir "\n"))
(insert (concat "export XWOS_AM_DIR=" xwos-am-dir "\n"))
(insert (concat "export XWOS_ARCH_DIR=" xwos-arch-dir "\n"))
(insert (concat "export XWOS_CPU_DIR=" xwos-cpu-dir "\n"))
(insert (concat "export XWOS_SOC_DIR=" xwos-soc-dir "\n"))
(insert (concat "export XWOS_PP_DIR=" xwos-pp-dir "\n"))
(insert (concat "export XWOS_BRD_DIR=" xwos-brd-dir "\n"))
(insert (concat "export XWOS_BDL_DIR=" xwos-bdl-dir "\n"))
(insert (concat "export XWOS_BM_DIR=" xwos-bm-dir "\n"))
(insert (concat "export XWOS_WKSPC_DIR=" xwos-wkspc-dir "\n"))
(insert "## ******** ******** ******** ******** includes ******** ******** ******** ******** ##\n")
(insert "source ${XWOS_PATH}/xwbs/util/sh/xwenv.sh\n")

(save-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; exit ;;;;;;;; ;;;;;;;; ;;;;;;;;
(kill-emacs 0)
