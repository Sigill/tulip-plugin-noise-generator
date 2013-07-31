#include <tulip/ImportModule.h>
#include <tulip/TulipPluginHeaders.h>
#include <stdexcept>

#define CHECK_PROP_PROVIDED(PROP, STOR) \
	do { \
		if(!dataSet->get(PROP, STOR)) \
		throw std::runtime_error(std::string("No \"") + PROP + "\" property provided."); \
	} while(0)

using namespace std;
using namespace tlp;

namespace {
	const char * paramHelp[] = {
		HTML_HELP_OPEN() \
			HTML_HELP_DEF( "type", "LayoutProperty" ) \
			HTML_HELP_BODY() \
			"The layout to add noise to." \
			HTML_HELP_CLOSE(),

		HTML_HELP_OPEN() \
			HTML_HELP_DEF( "type", "double" ) \
			HTML_HELP_BODY() \
			"Mean value of the noise." \
			HTML_HELP_CLOSE(),

		HTML_HELP_OPEN() \
			HTML_HELP_DEF( "type", "double" ) \
			HTML_HELP_BODY() \
			"Standard deviation of the noise." \
			HTML_HELP_CLOSE()
	};
}

class NoiseToLayout:public tlp::Algorithm {
private:
	tlp::LayoutProperty *layout;
	double mean, stddev;

public:
	PLUGININFORMATIONS("Add noise to layout", "Cyrille FAUCHEUX", "2012-01-17", "Add gaussian noise to a layout.", "1.0", "Layout");

	NoiseToLayout(const tlp::PluginContext *context):Algorithm(context) {
		addInParameter< LayoutProperty > ("layout", paramHelp[0], "viewLayout");
		addInParameter< double >         ("mean",   paramHelp[1], "0"         );
		addInParameter< double >         ("stddev", paramHelp[2], "1"         );
	}

	bool check(std::string &err) {
		try {
			if(dataSet == NULL)
				throw std::runtime_error("No dataset provided.");

			CHECK_PROP_PROVIDED("layout", this->layout);
			CHECK_PROP_PROVIDED("mean",   this->mean);
			CHECK_PROP_PROVIDED("stddev", this->stddev);

			if(this->stddev <= 0)
				throw std::runtime_error("The value for the \"stddev\" must be greater than 0.");
		} catch (std::runtime_error &ex) {
			err.assign(ex.what());
			return false;
		}

		return true;
	}

	bool run() {
		Iterator<node> *itNodes = graph->getNodes();
		node n;
		Coord node_layout;

		std::cout << "Mean is " << this->mean << std::endl;
		std::cout << "Stdev is " << this->stddev << std::endl;

		while(itNodes->hasNext()) {
			n = itNodes->next();
			node_layout = this->layout->getNodeValue(n);

			double d0 = gaussrand(this->mean, this->stddev);
			double d1 = gaussrand(this->mean, this->stddev);
			double d2 = gaussrand(this->mean, this->stddev);

			std::cout << "Node#" << n << std::endl;
			std::cout << "Adding " << d0 << " to " << node_layout[0] << std::endl;
			std::cout << "Adding " << d1 << " to " << node_layout[1] << std::endl;
			std::cout << "Adding " << d2 << " to " << node_layout[2] << std::endl;

			node_layout[0] = node_layout[0] + d0;
			node_layout[1] = node_layout[1] + d1;
			node_layout[2] = node_layout[2] + d2;

			std::cout << "Now " << node_layout[0] << std::endl;
			std::cout << "Now " << node_layout[1] << std::endl;
			std::cout << "Now " << node_layout[2] << std::endl;

			this->layout->setNodeValue(n, node_layout);
		}
		delete itNodes;

		return true;
	}

	// Borowed from http://c-faq.com/lib/gaussian.html
	double gaussrand(const double m, const double s) {
		static double V1, V2, S;
		static int phase = 0;
		double X;

		if(0 == phase) {
			do {
				double U1 = (double)rand() / RAND_MAX;
				double U2 = (double)rand() / RAND_MAX;

				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} while(S >= 1 || S == 0);

			X = V1 * sqrt(-2 * log(S) / S);
		} else
			X = V2 * sqrt(-2 * log(S) / S);

		phase = 1 - phase;

		return m + X * s;
	}

	~NoiseToLayout() {}
};

PLUGIN(NoiseToLayout)
