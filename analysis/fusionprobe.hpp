#ifndef _P2CL_FUSIONPROBE_HPP_
#define _P2CL_FUSIONPROBE_HPP_

#include <list>
#include "operation.hpp"
#include "parameter.hpp"
#include "opnode.hpp"

namespace p2cl{
	class FusionProbe{
		public:
			FusionProbe();
			
			void appendOp(OpNode * op);

			void update();

		private:
			const Parameter * lengthPara;

			OpNode * head;

			OpNode * tail;

			std::list<Operation *> opList;
	};
}

#endif
