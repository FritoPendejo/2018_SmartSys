INITIAL_NUM_RETRIES = 5


function RequestQueue ()

  local BUSY = false
  queue = {}

  function queue:processAt (position)

    if BUSY or position > table.maxn(self) then
      return nil
    else
      msg = (self[position])
    end

    url = msg.url
    data = msg.data

    -- the actual handler callback passed to the http module, it wraps the callback
    -- specified in the message, only calling it when the message is successful
    function handler (code, data)
      BUSY = false
      if code == 200 then
        msg.callback(code, data)
       -- log.info(msg.url, code)
        msg.numRetries = 0  -- it will be removed after this pass through
      else
        --log.warn("REQUEST FAILURE! code: ", code)
        -- log.warn("response data: ", pretty.write(data))
        -- log.warn("queue message: ", pretty.write(msg))
        msg.numRetries = msg.numRetries - 1
        if msg.numRetries == 0 then
          -- it will be removed after this pass through
          --log.error("Previous request failed for the last time ("..INITIAL_NUM_RETRIES.." times total), it will be dropped from the queue!")
        end
      end
      self:processAt(position + 1) -- recursively do the next item
    end

    -- set global lock
    BUSY = true

    -- msg.method is one of the `http` module methods, i.e. `http.get`, `http.post`, etc.
    msg.method(url, data, handler)
  end

  function queue:schedule (msg)
    msg.numRetries = INITIAL_NUM_RETRIES
    table.insert(self, msg)
  end

  function queue:cleanUp ()
    -- clear out expired messages from last time
    position = 1
    while table.maxn(self) >= position do
      if self[position].numRetries <= 0 then
        table.remove(self, position)
      else
        position = position + 1
      end
    end
  end

  function queue:its_showtime ()
    if BUSY then return end
    self:cleanUp()
    self:processAt(1)
  end

  return queue

end

return RequestQueue
