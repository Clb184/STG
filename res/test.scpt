
proc main {
	int a, b
	setl(a, 0.0)
	setl(b, 100.0)

	setpos(a, b)

	movepos(80, 1, 500.0, 200.0)

	nop
	func(10, 20, 30.0)
	printstack
	printlocals
	kill
}

proc func(int a, int b, float c) {
	setl(a, 30)
	setl(b, 50)
	setl(c, 120.0)

	pushl(c)
	pushl(a)
	i2f
	mulf
	subf
	popr(1)

	pushr(1)
	f2i
	popr(0)

	printlocals
}
