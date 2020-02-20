# DX7 Learning

(In progress)

This repository contains code for a deep learning approach to programming
the Yahama DX7 synthesizer. This project is part of the Stanford Deep
Learning Course [CS 230](https://cs230.stanford.edu/).

![](https://upload.wikimedia.org/wikipedia/commons/0/03/YAMAHA_DX7.jpg)

The Yamaha DX7 is a classic FM synthesizer, known for being notoriously
difficult to program. The goal of this project is to develop a system
that can use infer from a raw audio input, the best DX7 parameter set
to use to allow the recreation of that sound.

For generating data and testing our system, we are using the open-source
DX7 emulator [`dexed`](https://github.com/asb2m10/dexed).
