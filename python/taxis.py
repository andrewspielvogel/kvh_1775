import datetime as dt
import numpy as np

def time_since(time_axis, t0, time_interval):

  s = len(time_axis)

  for i in range(s):
    delta_t = dt.datetime.fromtimestamp(time_axis[i]) - t0
    secs = delta_t.total_seconds()
    if time_interval == 'seconds':
      time_axis[i] = secs
    elif time_interval == 'minutes':
      time_axis[i] = secs / 60
    elif time_interval == 'hours':
      time_axis[i] = secs / (60*60)
    elif time_interval == 'days':
      time_axis[i] = secs / (60*60*24)

  return time_axis[i]


def taxis(unix_time_axis):

  mn = np.min(unix_time_axis)
  mx = np.max(unix_time_axis)
  delta_t = mx - mn

  min_midnight = dt.datetime.fromtimestamp(mn)
  min_midnight = min_midnight.replace(hour=0, minute=0, second=0, microsecond=0)

  time_axis = unix_time_axis
  l = len(unix_time_axis)
 
  if delta_t <= 60*10:
    time_axis = time_since(unix_time_axis, min_midnight, 'seconds')
  elif delta_t <= 60*120:
    time_axis = time_since(unix_time_axis, min_midnight, 'minutes')
  elif delta_t <= 60*60*24:
    time_axis = time_since(unix_time_axis, min_midnight, 'hours')
  else:
    time_axis = time_since(unix_time_axis, min_midnight, 'days')

  return time_axis

