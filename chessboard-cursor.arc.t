(selective-load "mu.arc" section-level)

(reset)
(new-trace "read-move-legal")
(add-code:readfile "chessboard-cursor.mu")
(add-code
  '((function! main [
      (default-space:space-address <- new space:literal 30:literal/capacity)
      (1:channel-address/raw <- init-channel 3:literal)
      (r:integer/routine <- fork read-move:fn nil:literal/globals 200:literal/limit)
      (c:character <- copy ((#\a literal)))
      (x:tagged-value <- save-type c:character)
      (1:channel-address/raw/deref <- write 1:channel-address/raw x:tagged-value)
      (c:character <- copy ((#\2 literal)))
      (x:tagged-value <- save-type c:character)
      (1:channel-address/raw/deref <- write 1:channel-address/raw x:tagged-value)
      (c:character <- copy ((#\- literal)))
      (x:tagged-value <- save-type c:character)
      (1:channel-address/raw/deref <- write 1:channel-address/raw x:tagged-value)
      (c:character <- copy ((#\a literal)))
      (x:tagged-value <- save-type c:character)
      (1:channel-address/raw/deref <- write 1:channel-address/raw x:tagged-value)
      (c:character <- copy ((#\4 literal)))
      (x:tagged-value <- save-type c:character)
      (1:channel-address/raw/deref <- write 1:channel-address/raw x:tagged-value)
      (sleep until-routine-done:literal r:integer/routine)
     ])))
;? (set dump-trace*)
;? (= dump-trace* (obj whitelist '("schedule")))
(run 'main)
(each routine completed-routines*
  (awhen rep.routine!error
;?     (prn "  " r)
    (prn "error - " it)))
(if (~ran-to-completion 'read-move)
  (prn "F - chessboard accepts legal moves (<rank><file>-<rank><file>)"))
;? (quit)

(reset)
(new-trace "read-move-incomplete")
(add-code:readfile "chessboard-cursor.mu")
(add-code
  '((function! main [
      (default-space:space-address <- new space:literal 30:literal/capacity)
      (1:channel-address/raw <- init-channel 3:literal)
      (r:integer/routine <- fork-helper read-move:fn nil:literal/globals 200:literal/limit)
      (c:character <- copy ((#\a literal)))
      (x:tagged-value <- save-type c:character)
      (1:channel-address/raw/deref <- write 1:channel-address/raw x:tagged-value)
      (c:character <- copy ((#\2 literal)))
      (x:tagged-value <- save-type c:character)
      (1:channel-address/raw/deref <- write 1:channel-address/raw x:tagged-value)
      (c:character <- copy ((#\- literal)))
      (x:tagged-value <- save-type c:character)
      (1:channel-address/raw/deref <- write 1:channel-address/raw x:tagged-value)
      (c:character <- copy ((#\a literal)))
      (x:tagged-value <- save-type c:character)
      (1:channel-address/raw/deref <- write 1:channel-address/raw x:tagged-value)
      (sleep until-routine-done:literal r:integer/routine)
     ])))
(run 'main)
(if (ran-to-completion 'read-move)
  (prn "F - chessboard hangs until 5 characters are entered"))

(reset)
(new-trace "read-move-quit")
(add-code:readfile "chessboard-cursor.mu")
(add-code
  '((function! main [
      (default-space:space-address <- new space:literal 30:literal/capacity)
      (1:channel-address/raw <- init-channel 3:literal)
      (r:integer/routine <- fork-helper read-move:fn nil:literal/globals nil:literal/limit)
      (c:character <- copy ((#\q literal)))
      (x:tagged-value <- save-type c:character)
      (1:channel-address/raw/deref <- write 1:channel-address/raw x:tagged-value)
      (sleep until-routine-done:literal r:integer/routine)
     ])))
(run 'main)
(if (~ran-to-completion 'read-move)
  (prn "F - chessboard quits on move starting with 'q'"))

(reset)
