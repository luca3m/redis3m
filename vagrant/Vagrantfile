# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

HOSTS = { "redis01" => "192.168.57.2",
          "redis02" => "192.168.57.3",
          "redis03" => "192.168.57.4"
}

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  HOSTS.each do |r|
    config.vm.define r[0] do |rabbit|
      rabbit.vm.provision 'shell', path: "provision.sh"
      rabbit.vm.box = "precise32"
      rabbit.vm.box_url = "http://files.vagrantup.com/precise32.box"
      rabbit.vm.network :private_network, ip: r[1]
      rabbit.vm.hostname = r[0]
    end
  end
end
