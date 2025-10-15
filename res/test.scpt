
fn main {
	int a
	int b
	SetL(a, 0.0)
	SetL(b, 100.0)

	PushL(a)
	PushL(b)
	SetPos()

	PushC(200.0)
	PushC(200.0)
	PushC(300.0)
	AddF()
	PushC(1)
	PushC(80)
	MovePos()

	Nop()
	PushC(30.0)
	PushC(20)
	PushC(10)
	Enter(3)
	Call(func)
	Leave()
	Kill()
}

fn func(int a, int b, float c) {
	SetL(a, 30)
	SetL(b, 50)
	SetL(c, 120.0)

}
