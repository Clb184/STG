fn main {
	Enter(2)
	
	SetL(0, 0.0)
	SetL(1, 100.0)

	PushL(1)
	PushL(0)
	SetPos()

	PushC(200.0)
	PushC(200.0)
	PushC(300.0)
	AddF()
	PushC(1)
	PushC(80)
	MovePos()

	Nop()

	PushL(1)
	PushL(0)
	PushC(3.0)
	MulF()
	AddF()
	PopR(1)
	PushR(1)
	F2I()
	PushC(23)
	AddI()
	PopR(0)

	Kill()
}

fn func(int a) {

}