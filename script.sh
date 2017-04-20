#!/bin/bash
for i in {0..19}
{
	./nodes 127.0.0.1 8888&
}
exit 0