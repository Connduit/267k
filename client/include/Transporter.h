#ifndef TRANSPORTER_H
#define TRANSPORTER_H

class Transporter
{
public:
	// constructor
	// deconstructor
	virtual ~Transporter() = default; // TODO: what does default do?
private:
	const char* host_;
	const char* port_;
};

typedef std::unique_ptr<Transporter> TransporterUniquePtr;

class TCPTransporter : public Transporter
{
public:
private:
};

#endif
