#! /bin/sh
":"; exec emacs --quick --script "$0" -- "$@" # -*- mode: emacs-lisp; lexical-binding: t; -*-
;; @file
;; @brief Emacs-script to update XuanWuKO version
;; @author
;; + 隐星魂 (Roy.Sun) <www.starsoul.tech>
;; @copyright
;; + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
;; > Licensed under the Apache License, Version 2.0 (the "License");
;; > you may not use this file except in compliance with the License.
;; > You may obtain a copy of the License at
;; >
;; >         http://www.apache.org/licenses/LICENSE-2.0
;; >
;; > Unless required by applicable law or agreed to in writing, software
;; > distributed under the License is distributed on an "AS IS" BASIS,
;; > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;; > See the License for the specific language governing permissions and
;; > limitations under the License.

(defvar elscript (expand-file-name (nth 2 command-line-args)) "script name")
(defvar elpath (file-name-directory elscript))
(push elpath load-path)
(require 'errno)

;;;;;;;; ;;;;;;;; ;;;;;;;; config ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defconst cfg-log-lv 0)

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

;;;;;;;; ;;;;;;;; ;;;;;;;; misc function ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defun expand-directory (dir &rest arglist)
  (file-name-directory
   (expand-file-name
    (concat dir
            (let (result)
              (dolist (elt arglist result)
                (setq result (concat result elt))))
            "fakefile"))))

(defun oddp (num)
  (if (= (% num 2) 0) nil t))

(defun evenp (num)
  (if (= (% num 2) 0) t nil))

;;;;;;;; ;;;;;;;; ;;;;;;;; arguments ;;;;;;;; ;;;;;;;; ;;;;;;;;
(setq debug-on-error t)
(setq make-backup-files nil)
(defvar argc (length argv) "Count of arguments")
(defvar opt-major nil "increase major")
(defvar opt-minor nil "increase minor")
(defvar opt-revision nil "increase revision")
(defvar opt-reset nil "Reset change")
;;(logi "argv:%s" argv)

(catch 'break
  (pop argv)  ; Remove first "--" separator
  (while argv
    (let ((option (pop argv)))
      ;; (logd "option:%s" option)
      (cond
       ;; options after second "--"
       ((string= option "--") (throw 'break nil))
       ;; -M
       ;; --major
       ((string= option "--major") (setq opt-major t))
       ((string= option "-M") (setq opt-major t))
       ;; -m
       ;; --minor
       ((string= option "--minor") (setq opt-minor t))
       ((string= option "-m") (setq opt-minor t))
       ;; -v
       ;; --revision
       ((string= option "--revision") (setq opt-revision t))
       ((string= option "-v") (setq opt-revision t))
       ;; -r
       ;; --reset
       ((string= option "--reset") (setq opt-reset t))
       ((string= option "-r") (setq opt-reset t))
       ;; Unknown option --*
       ((string-prefix-p "--" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))
       ;; Unknown option -*
       ((string-prefix-p "-" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))
       ;; Unknown option *
       (t
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))))))

;;;;;;;; ;;;;;;;; ;;;;;;;; env ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defvar XuanWuKO-topdir (expand-directory elpath "../../../"))
(defvar XuanWuKO-kndir (expand-directory XuanWuKO-topdir "xwos/"))
(defvar XuanWuKO-version-file (expand-file-name "version.h" XuanWuKO-kndir))

;;(logd "XuanWuKO-topdir:%s" XuanWuKO-topdir)
;;(logd "XuanWuKO-version-file:%s" XuanWuKO-version-file)

(setq version-file-buffer (find-file-noselect XuanWuKO-version-file))
(set-buffer version-file-buffer)
(set-buffer-multibyte t)

(goto-char (point-min))
(re-search-forward "^#define[ \t]+\\(XWOS_VERSION_MAJOR\\)[ \t]+\\(.+\\)")
(setq XuanWuKO-version-major (match-string 2))

(goto-char (point-min))
(re-search-forward "^#define[ \t]+\\(XWOS_VERSION_MINOR\\)[ \t]+\\(.+\\)")
(setq XuanWuKO-version-minor (match-string 2))

(goto-char (point-min))
(re-search-forward "^#define[ \t]+\\(XWOS_VERSION_REVISION\\)[ \t]+\\(.+\\)")
(setq XuanWuKO-version-revision (match-string 2))

(logi "current version: V%s.%s.%s"
      XuanWuKO-version-major XuanWuKO-version-minor XuanWuKO-version-revision)

(if opt-reset
  ;; then
  (let (recent-git-commit vnum-major vnum-minor vnum-revision vstr)
    (setq recent-git-commit
          (with-temp-buffer
            (call-process "git" nil t nil "log" "-1")
            (buffer-string)))
    (setq vnum-major (string-to-number XuanWuKO-version-major))
    (setq vnum-minor (string-to-number XuanWuKO-version-minor))
    (setq vnum-revision (string-to-number XuanWuKO-version-revision))
    (setq vstr (concat XuanWuKO-version-major "."
                       XuanWuKO-version-minor "."
                       XuanWuKO-version-revision))
    (cond
     ((or (string-match "patch: :bookmark:" recent-git-commit)
          (string-match "release: :bookmark:" recent-git-commit)
          (string-match "developing: :construction:" recent-git-commit)
          (string-match "milestone: :bookmark:" recent-git-commit))
      (call-process "git" nil nil nil "tag" "-d" (concat "XuanWuKO-V" vstr))
      (call-process "git" nil nil nil "reset" "HEAD^")
      (call-process "git" nil nil nil "checkout" XuanWuKO-version-file)
      (logi "Reset version .... [OK]"))
     (t (loge "[FAILED] The recent git commit is not a version update!"))))
  ;; else
  (let (vnum-major vnum-minor vnum-revision vstr)
    (setq vnum-major (string-to-number XuanWuKO-version-major))
    (setq vnum-minor (string-to-number XuanWuKO-version-minor))
    (setq vnum-revision (string-to-number XuanWuKO-version-revision))
    (cond
     (opt-major ;; case opt-major
      (setq XuanWuKO-version-major (number-to-string (+ vnum-major 1)))
      (setq XuanWuKO-version-minor "0")
      (setq XuanWuKO-version-revision "0")
      (setq vstr (concat XuanWuKO-version-major "."
                         XuanWuKO-version-minor "."
                         XuanWuKO-version-revision))
      (set-buffer version-file-buffer)
      (set-buffer-multibyte t)
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_MAJOR[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XuanWuKO-version-major)))
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_MINOR[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XuanWuKO-version-minor)))
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_REVISION[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XuanWuKO-version-revision)))
      (save-buffer)
      (call-process "git" nil nil nil "add" XuanWuKO-version-file)
      (call-process "git" nil nil nil
                    "commit" "-m" (concat "milestone: :bookmark: 玄武KO-V" vstr))
      (call-process "git" nil nil nil "tag" "-a"
                    "-m" (concat "XuanWuKO-" vstr) (concat "XuanWuKO-V" vstr))
      (logi "next version: V%s" vstr))
     (opt-minor ;; case opt-minor
      (set-buffer version-file-buffer)
      (set-buffer-multibyte t)
      (setq XuanWuKO-version-minor (number-to-string (+ vnum-minor 1)))
      (setq vstr (concat XuanWuKO-version-major "."
                         XuanWuKO-version-minor "."
                         XuanWuKO-version-revision))
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_MINOR[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XuanWuKO-version-minor)))
      (setq XuanWuKO-version-revision "0")
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_REVISION[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XuanWuKO-version-revision)))
      (save-buffer)
      (call-process "git" nil nil nil "add" XuanWuKO-version-file)
      (call-process "git" nil nil nil
                    "commit" "-m" (concat "developing: :construction: 玄武KO-V" vstr))
      (if (oddp vnum-minor)
          (call-process "git" nil nil nil "tag" "-a"
                        "-m" (concat "XuanWuKO-" vstr) (concat "XuanWuKO-V" vstr)))
      (logi "next version: V%s" vstr))
     (opt-revision ;; case opt-revision
      (setq XuanWuKO-version-revision (number-to-string (+ vnum-revision 1)))
      (setq vstr (concat XuanWuKO-version-major "."
                         XuanWuKO-version-minor "."
                         XuanWuKO-version-revision))
      (set-buffer version-file-buffer)
      (set-buffer-multibyte t)
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_REVISION[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XuanWuKO-version-revision)))
      (save-buffer)
      (call-process "git" nil nil nil "add" XuanWuKO-version-file)
      (call-process "git" nil nil nil
                    "commit" "-m" (concat "patch: :bookmark: 玄武KO-V" vstr))
      (if (evenp vnum-minor)
          (call-process "git" nil nil nil "tag" "-a"
                        "-m" (concat "XuanWuKO-" vstr) (concat "XuanWuKO-V" vstr)))
      (logi "next version: V%s" vstr)))))

;;;;;;;; ;;;;;;;; ;;;;;;;; exit ;;;;;;;; ;;;;;;;; ;;;;;;;;
(kill-emacs 0)
