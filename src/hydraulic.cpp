#include <math.h>
#include <assert.h>
#include <vector>
#include <lib/reynolds.h>

class Line;
class Node;

#define mm	* 0.001

typedef std::vector<Line *> Lines;
typedef std::vector<Node *> Nodes;

class Line {
public:
	Line(): m_inConn(0), m_outConn(0) { Line::add(this); }
	virtual ~Line() { Line::remove(this); }

	void append(Line * other);
	void prepend(Line * other);
	void inNode(Node * node) { assert(!m_inConn); m_inConn = node; }
	void outNode(Node * node) { assert(!m_outConn); m_outConn = node; }
	Node * inNode() const { return m_inConn; }
	Node * outNode() const { return m_outConn; }
	static const Lines & instances() { return Line::s_insts; }
	virtual double flow() const = 0;
	virtual double tempLoss(double outTemp) const = 0;
	virtual double pressureLoss() const = 0;

protected:
	static void add(Line * line) { Line::s_insts.push_back(line); }
	static void remove(Line * line) { return; }

protected:
	Node * m_inConn, * m_outConn;
	static Lines s_insts;
};

std::vector<Line *> Line::s_insts;

class FlowConductor: public Line {
public:
	FlowConductor(): m_flow(0) {}
	void flow(double flow) { m_flow = flow; }
	double flow() const { return m_flow; }

protected:
	double m_flow;
};

class FlowProvider: public Line {
};

class Pipe: public FlowConductor {
public:
	Pipe(double diameter, double wall, double length): m_diameter(diameter), m_wall(wall), m_length(length) {}
	double flowSpeed(double flow) const;
	double tempLoss(double outTemp) const;
	double pressureLoss() const;

protected:
	double m_diameter;
	double m_wall;
	double m_length;
};

class Node {
public:
	Node(): m_temp(0), m_pressure(0) { Node::add(this); }
	Node(Line * in, Line * out): m_temp(0), m_pressure(0) { this->in(in); this->out(out); Node::add(this); }
	virtual ~Node() { Node::remove(this); }

	void temperature(double temp) { m_temp = temp; }
	double temperature() const { return m_temp; }
	void pressure(double press) { m_pressure = press; }
	double pressure() const { return m_pressure; }
	void in(Line * in) { m_inputs.push_back(in); in->outNode(this); }
	void out(Line * out) { m_outputs.push_back(out); out->inNode(this); }
	const Lines & inputs() const { return m_inputs; }
	const Lines & outputs() const { return m_outputs; }

	static const Nodes & instances() { return Node::s_insts; }

protected:
	static void add(Node * n) { Node::s_insts.push_back(n); }
	static void remove(Node * n) { return; }

protected:
	double m_temp;
	double m_pressure;
	Lines m_inputs;
	Lines m_outputs;

	static Nodes s_insts;
};

std::vector<Node *> Node::s_insts;

class Radiator: public FlowProvider {
public:
	Radiator(double power): m_power(power) {}
	double flow() const;
	double tempLoss(double outTemp) const;
	double pressureLoss() const;

protected:
	double m_power;
};

void Line::append(Line * other) {
	if (!m_outConn) {
		new Node(this, other);
	} else {
		m_outConn->out(other);
	}
}

void Line::prepend(Line * other) {
	if (!m_inConn) {
		new Node(other, this);
	} else {
		m_inConn->in(other);
	}
}

double Pipe::flowSpeed(double flow) const {
	double surface = M_PI * pow(m_diameter / 2.0, 2.0);
	double flowMeters = flow / 985.7; 
	return flowMeters / surface;
}

double Pipe::tempLoss(double outTemp) const {
	double R = m_wall / 372;
	double dT = inNode()->temperature() - 20;
	double fi = dT / R;
	printf("Flow is %f W/m\n", fi);
	double surface = 2.0 * M_PI * ((m_diameter / 2.0) + (m_wall / 2.0)) * m_length;
	printf("Surface is %f m2\n", surface);
	double volume = M_PI * pow(m_diameter / 2.0, 2) * m_length;
	double power = fi * surface;
	double energy = power / (m_length / flowSpeed(m_flow));

	return (volume * 985.7 * 4186) / energy;
}

double __a(double Re, double epsylon) {
	return pow(
			-2.457 * log(
					pow(7.0/Re, 0.9) + 0.27 * epsylon
				)
			,16
			);
}

double __b(double Re) {
	return pow(37530.0/Re, 16);
}

double friction_coeff(double Re, double d, double k) {
	return 8.0 * pow(
			pow(8.0/Re, 12) + (1.0f/ pow(__a(Re, k/d) + __b(Re), 1.5))
			, (1.0/12.0)
			);
}

double Pipe::pressureLoss() const {
	double Re = Reynolds::number(m_diameter, flowSpeed(m_flow), 985.7, 0.000547);
	printf("Reynolds number for diameter %f m and flow speed %f m/s (flow %f kg/s) is %f\n", m_diameter, flowSpeed(m_flow), m_flow, Re);
	double lambda = friction_coeff(Re, m_diameter, 0.000003);
	return lambda * (m_length/m_diameter) * (pow(flowSpeed(m_flow), 2.0) / 2.0) * 985.7;
}

double Radiator::flow() const {
	double inTemp = inNode()->temperature();
	double outTemp = outNode()->temperature();
	double deltaP = inNode()->pressure() - outNode()->pressure();
	double flow = m_power / (4186 * (inTemp - outTemp));
	printf("Radiator stats\n==============\nDelta t = %f deg. C\tPower = %.2f W \tFlow %.3f kg/s\tPressure = %f Pa\n\n", (inTemp - outTemp), m_power, flow, deltaP);
	return flow;
}

double Radiator::tempLoss(double outTemp) const {
	return inNode()->temperature() - outNode()->temperature();
}

double Radiator::pressureLoss() const { 
	return inNode()->pressure() - outNode()->pressure();
}

Radiator * genRadiator(Pipe * input, Pipe * output, double power, double diameter, double wall, double l1, double l2) {
	Pipe * inLine = new Pipe(diameter, wall, l1);
	Pipe * outLine = new Pipe(diameter, wall, l2);
	Radiator * r = new Radiator(power);
	r->prepend(inLine);
	r->append(outLine);
	input->append(inLine);
	output->prepend(outLine);
	return r;
}

// Sets node temperature to fixed value until it reaches radiator which is not traversed in neither direction

void forwardTemperature(double temp, Node * start) {
	if (!start) return;
	start->temperature(temp);
	const Lines & outputs = start->outputs();
	for (Lines::const_iterator it = outputs.begin(); it != outputs.end(); ++it) {
		Radiator * r = dynamic_cast<Radiator *>(*it);
		if (r) continue;		// this is radiator, do not traverse it
		forwardTemperature(temp, (*it)->outNode());
	}
	return;
}

double forwardFlow(Node * start) {
	double thisFlow = 0;
	const Lines & outs = start->outputs();
	for (Lines::const_iterator it = outs.begin(); it != outs.end(); ++it) {
		FlowProvider * r = dynamic_cast<FlowProvider *>(*it);
		if (r) thisFlow += r->flow();
		else {
			FlowConductor * c = dynamic_cast<FlowConductor *>(*it);
			if (c) {
				double subFlow = forwardFlow(c->outNode());
				c->flow(subFlow);
				thisFlow += subFlow;
			}
		}
	}
	return thisFlow;
}

double reverseFlow(Node * start) {
	double thisFlow = 0;
	const Lines & ins = start->inputs();
	for (Lines::const_iterator it = ins.begin(); it != ins.end(); ++it) {
		FlowProvider * r = dynamic_cast<FlowProvider *>(*it);
		if (r) thisFlow += r->flow();
		else {
			FlowConductor * c = dynamic_cast<FlowConductor *>(*it);
			if (c) {
				double subFlow = reverseFlow(c->inNode());
				c->flow(subFlow);
				thisFlow += subFlow;
			}
		}
	}
	return thisFlow;
}

void forwardTemperatureLoss(double outTemp, Node * start) {
	if (!start) return;
	const Lines & outputs = start->outputs();
	for (Lines::const_iterator it = outputs.begin(); it != outputs.end(); ++it) {
		Radiator * r = dynamic_cast<Radiator *>(*it);
		if (r) continue;		// this is radiator, do not traverse it
		double deltaT = (*it)->tempLoss(outTemp);
		(*it)->outNode()->temperature(start->temperature() - deltaT);
		printf("Delta T is %f\tEndpoint temperature is %f\n", deltaT, (*it)->outNode()->temperature());
	
		forwardTemperatureLoss(outTemp, (*it)->outNode());
	}
	return;

}

void reverseTemperatureLoss(double outTemp, Node * start) {
	if (!start) return;
	const Lines & inputs = start->inputs();
	for (Lines::const_iterator it = inputs.begin(); it != inputs.end(); ++it) {
		Radiator * r = dynamic_cast<Radiator *>(*it);
		if (r) continue;		// this is radiator, do not traverse it
		double deltaT = (*it)->tempLoss(outTemp);
		(*it)->inNode()->temperature(start->temperature() + deltaT);
		reverseTemperatureLoss(outTemp, (*it)->inNode());
	}
	return;

}

void forwardPressureLoss(Node * start) {
	if (!start) return;
	const Lines & outputs = start->outputs();
	for (Lines::const_iterator it = outputs.begin(); it != outputs.end(); ++it) {
		Radiator * r = dynamic_cast<Radiator *>(*it);
		if (r) continue;		// this is radiator, do not traverse it
		double deltaP = (*it)->pressureLoss();
		(*it)->outNode()->pressure(start->pressure() - deltaP);
		printf("Delta p is %f\nEndpoint poressure is %f\n mbar", deltaP, (*it)->outNode()->temperature() / 1000.0f);
	
		forwardPressureLoss((*it)->outNode());
	}
	return;

}

void reversePressureLoss(Node * start) {
	if (!start) return;
	const Lines & inputs = start->inputs();
	for (Lines::const_iterator it = inputs.begin(); it != inputs.end(); ++it) {
		Radiator * r = dynamic_cast<Radiator *>(*it);
		if (r) continue;		// this is radiator, do not traverse it
		double deltaP = (*it)->pressureLoss();
		(*it)->inNode()->pressure(start->pressure() + deltaP);
		reversePressureLoss((*it)->inNode());
	}
	return;

}


void reverseTemperature(double temp, Node * start) {
	if (!start) return;
	start->temperature(temp);
	const Lines & inputs = start->inputs();
	for (Lines::const_iterator it = inputs.begin(); it != inputs.end(); ++it) {
		Radiator * r = dynamic_cast<Radiator *>(*it);
		if (r) continue;		// this is radiator, do not traverse it
		reverseTemperature(temp, (*it)->inNode());
	}
	return;
}

/*
void calcRadiatorFlows() {
	const Lines & l = Line::instances();
	for (Lines::const_iterator it = l.begin(); it != l.end(); ++it) {
		Radiator * r = dynamic_cast<Radiator *>(*it);
		if (r) r->flow();
	}
}*/

int main(int argc, char ** argv) {
	Node * systemInput = new Node();
	Node * systemOutput = new Node();

	systemInput->pressure(20000);
	systemOutput->pressure(0);

	// "stupacka"
	Pipe * p0 = new Pipe(16 mm, 1 mm, 1.5);
	// vyvod radiator dielna
	Pipe * p1 = new Pipe(16 mm, 1 mm, 1.8);
	// vyvod radiator hostovska
	Pipe * p2 = new Pipe(16 mm, 1 mm, 0.95);
	// vyvod radiator kupelna
	Pipe * p3 = new Pipe(16 mm, 1 mm, 2.6 + 0.2 + 1.54 + 0.5 + 1.0);
	// vyvod radiator spalna
	Pipe * p4 = new Pipe(16 mm, 1 mm, 3.8 + 0.2 + 0.1);
	// vyvod radiator kuchyna
	Pipe * p5 = new Pipe(16 mm, 1 mm, 2.0 + 0.2 + 1.5);
	// vyvod radiator obyvacka

	systemInput->out(p0);
	p0->append(p1);
	p1->append(p2);
	p2->append(p3);
	p3->append(p4);
	p4->append(p5);


	// "spiatocka"
	Pipe * p6 = new Pipe(16 mm, 1 mm, 1.5);
	// vyvod radiator dielna
	Pipe * p7 = new Pipe(16 mm, 1 mm, 1.8);
	// vyvod radiator hostovska
	Pipe * p8 = new Pipe(16 mm, 1 mm, 0.95);
	// vyvod radiator kupelna
	Pipe * p9 = new Pipe(16 mm, 1 mm, 2.6 + 0.2 + 1.54 + 0.5 + 1.0);
	// vyvod radiator spalna
	Pipe * p10 = new Pipe(16 mm, 1 mm, 3.8 + 0.2 + 0.1);
	// vyvod radiator kuchyna
	Pipe * p11 = new Pipe(16 mm, 1 mm, 2.0 + 0.2 + 1.5);
	// vyvod radiator obyvacka

	systemOutput->in(p6);
	p6->prepend(p7);
	p7->prepend(p8);
	p8->prepend(p9);
	p9->prepend(p10);
	p10->prepend(p11);

	// radiator obyvacka juh
	Radiator * rOR1 = genRadiator(p5, p11, 1000, 10 mm, 1 mm, 3.4 + 1.4 + 1.6, 3.4 + 1.4 + 2.1);

	// radiator obyvacka vychod
	Radiator * rOR2 = genRadiator(p5, p11, 1000, 10 mm, 1 mm, 1.6, 2.1);

	// radiator kuchyna
	Radiator * rKuch = genRadiator(p4, p10, 601, 10 mm, 1 mm, 1.6, 2.1);

	// radiator spalna
	Radiator * rSpal = genRadiator(p3, p9, 1520, 10 mm, 1 mm, 1.6, 2.1);

	// radiator kupelna
	Radiator * rKup = genRadiator(p2, p8, 364, 10 mm, 1 mm, 1.6, 2.1);

	// radiator hostovska
	Radiator * rHost = genRadiator(p1, p7, 690, 10 mm, 1 mm, 6.0 + 1.6, 6.0 + 2.1);

	// radiator dielna
	Radiator * rDieln = genRadiator(p0, p6, 922, 10 mm, 1 mm, 0.5, 1.0);

	forwardTemperature(55, systemInput);
	reverseTemperature(45, systemOutput);

	{
	const Nodes & n = Node::instances();
	for (Nodes::const_iterator it = n.begin(); it != n.end(); ++it) {
		printf("Node temperature is %f\n", (*it)->temperature());
	}
	}

//	calcRadiatorFlows();

	double inputFlow = forwardFlow(systemInput);
	double outputFlow = reverseFlow(systemOutput);

	if (inputFlow != outputFlow) {
		printf("WTF? Different flow? (%f vs. %f)\n", inputFlow, outputFlow);
	} else {
		printf("Overal system flow is %f kg/s\n", inputFlow);
	}

	forwardTemperatureLoss(20.0, systemInput);
	reverseTemperatureLoss(20.0, systemOutput);

	{
	const Nodes & n = Node::instances();
	for (Nodes::const_iterator it = n.begin(); it != n.end(); ++it) {
		printf("Node temperature is %f\n", (*it)->temperature());
	}
	}

	forwardPressureLoss(systemInput);
	reversePressureLoss(systemOutput);

	inputFlow = forwardFlow(systemInput);
	outputFlow = reverseFlow(systemOutput);

	return 0;
}
