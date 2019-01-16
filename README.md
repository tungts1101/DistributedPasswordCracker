# Distributed Password Cracker

  

## A system consists of 3 parts

  

* A server: gets request from requester and distributes job to workers

* A requester: requests server to solve a hash password

* A worker: solves hash password

  

## System usage

  
Clone Project
`$ git clone git@github.com:helloworld1101/DistributedPasswordCracker.git`

  

Server Usage
`$ ./bin/server`
(Type -h for helper menu)



Requester Usage
`$ ./bin/requester <server-addr> <server-port>`

  

Worker Usage
`$ ./bin/worker <server-addr> <server-port>`
Type JOIN to join the network