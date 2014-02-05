host{'redis01':
  ensure => present,
  ip => "192.168.57.2",
}

host{'redis02':
  ensure => present,
  ip => "192.168.57.3",
}

host{'redis03':
  ensure => present,
  ip => "192.168.57.4",
}

class{'redis':
  version => "2.8.3",
}
