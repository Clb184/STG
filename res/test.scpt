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
	Kill()
}

fn func(int a) {

}