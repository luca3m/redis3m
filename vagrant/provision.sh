#!/bin/bash
VERSION="2.8.5"

# install build tools
sudo apt-get update
sudo apt-get install -y build-essential

# update hosts
cat /vagrant/redis-hosts.txt | sudo tee -a /etc/hosts

# install redis
wget http://download.redis.io/releases/redis-$VERSION.tar.gz
tar xfz redis-$VERSION.tar.gz
rm redis-$VERSION.tar.gz
cd redis-$VERSION
make
sudo make install
cd ..
rm -fr redis-$VERSION

# Configure redis db
sudo cp /vagrant/redis.upstart.conf /etc/init/redis.conf
sudo cp /vagrant/redis.conf /etc/redis.conf
sudo service redis start

# configure redis sentinel
sudo cp /vagrant/redis-sentinel.upstart.conf /etc/init/redis-sentinel.conf
sudo cp /vagrant/redis-sentinel.conf /etc/redis-sentinel.conf
sudo service redis-sentinel start
