#!/bin/bash
for i in {0..3}
{
	./process 127.0.0.1 $((i)) 4&
}
exit 0