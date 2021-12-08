<p align="center">
<img src="https://img.shields.io/static/v1?label=PROGRAM&message=C&color=E71262&style=for-the-badge&logo=ghost"
</p>

![alt text](pictures/bannerAurras.png)

<h2 align="center">
  &#128314; Aurras &#128314;
</h2>

<h5 align="center">
  Made for your ears to discover new sensations
</h5>

<h5 align="center">
  &#128296; In progress &#128296;
</h5>

Summary
=======

* [About project](#about-project)
* [User guide](#user-guide)
* [Authors](#authors)
<br>

### About project

A project developed for the subject of Operating Systems, whose purpose is to process audio files, applying various filters. Multiple users can request filters to be applied in a particular order and can request information about the tasks being executed. The service, which allows for concurrent requests, comprises a Client and a Server which communicate via named pipes.


### User guide
##### Compiling
```sh
make
```
##### Running Server
```sh
bin/aurrasd config-filename filters-folder
```
##### Example of a Client Request
```sh
bin/aurras transform samples/sample-1.m4a output.m4a alto eco rapido
```
<br>

### Authors

Henrique Costa
<br>
Matilde Bravo
<br>
Mariana Rodrigues
