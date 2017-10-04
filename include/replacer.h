
#ifndef _REPLACER_H_
#define _REPLACER_H_

class Replacer {

public:

	Replacer();

	// This function returns the frame(page) of the victim to be replaced.
	virtual int PickVictim() = 0;

	// This function adds frame to the list of candidates to be replaced.
	virtual void AddFrame(int frameId) = 0;

	// This function removes frame from the list of candidates to be replaced.
	virtual void RemoveFrame(int frameId) = 0;


	virtual ~Replacer() = 0;
};

#endif // _REPLACER