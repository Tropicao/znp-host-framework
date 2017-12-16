# Todo
* Add "module management" : it would be nice to be able to disable all logs from
  the framework, but let the app log at desired level.
* MT Parser : log if incoming MT message is POLL, SREQ, AREQ or SRSP
* Dirty hack found in Transport : sleep 1ms after each write ?!
* Check if rpcForceRun is used. If not => delete
* More usleep in code
* Check if SRSP is still properly managed
* Param ((unused)) in library : delete
* No that lib is monothreaded : do we really need a queue for incoming messages
* fix dbg_print for no line return request
