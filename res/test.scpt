
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
	Kill()
}

fn func(int a, int b, float c) {

}
