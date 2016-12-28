/*
 * Copyright © 2016  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   callifsimplifier.cc
 */

#include "callifsimplifier.h"

namespace stmi
{

namespace CallIfSimplifier
{

const shared_ptr<CallIf> simplify(
						const shared_ptr<CallIf>& refCallIf, const Event::Class& oEventClass)
{
//std::cout << "CallIfSimplifier::simplify  oEventClass=" << oEventClass.getId() << std::endl;
	const CallIf& oCallIf = *refCallIf;
	const std::type_info& oCallIfType = typeid(oCallIf);
	if (oCallIfType == typeid(CallIfEventClass)) { // this can be done because the class is final
		auto p0ECF = static_cast<CallIfEventClass*>(refCallIf.get());
		if (p0ECF->getClass() == oEventClass) {
			return CallIfTrue::getInstance(); //--------------------------------
		} else {
			return CallIfFalse::getInstance(); //-------------------------------
		}
	} else if (oCallIfType == typeid(CallIfXYEvent)) {
		if (oEventClass.isXYEvent()) {
			return CallIfTrue::getInstance(); //--------------------------------
		} else {
			return CallIfFalse::getInstance(); //-------------------------------
		}
	} else if (oCallIfType == typeid(CallIfAnd)) {
		auto p0AndF = static_cast<CallIfAnd*>(refCallIf.get());
		auto refSF1 = simplify(p0AndF->getCallIf1(), oEventClass);
		auto refSF2 = simplify(p0AndF->getCallIf2(), oEventClass);
		const CallIf& oSF1 = *refSF1;
		const CallIf& oSF2 = *refSF2;
		const std::type_info& oSF1Type = typeid(oSF1);
		const std::type_info& oSF2Type = typeid(oSF2);
		if (oSF1Type == typeid(CallIfTrue)) {
			// And(True,Op2) = Op2
			return refSF2; //---------------------------------------------------
		} else if (oSF2Type == typeid(CallIfTrue)) {
			// And(Op1,True) = Op1
			return refSF1; //---------------------------------------------------
		} else if ((oSF1Type == typeid(CallIfFalse)) || (oSF2Type == typeid(CallIfFalse))) {
			// And(False,Op2) = False,  And(Op1,False) = False
			return CallIfFalse::getInstance(); //-------------------------------
		}
	} else if (oCallIfType == typeid(CallIfOr)) {
		auto p0OrF = static_cast<CallIfOr*>(refCallIf.get());
		auto refSF1 = simplify(p0OrF->getCallIf1(), oEventClass);
		auto refSF2 = simplify(p0OrF->getCallIf2(), oEventClass);
		const CallIf& oSF1 = *refSF1;
		const CallIf& oSF2 = *refSF2;
		const std::type_info& oSF1Type = typeid(oSF1);
		const std::type_info& oSF2Type = typeid(oSF2);
		if (oSF1Type == typeid(CallIfFalse)) {
			// Or(False, Op2) = Op2
			return refSF2; //---------------------------------------------------
		} else if (oSF2Type == typeid(CallIfFalse)) {
			// Or(Op1,False) = Op1
			return refSF1; //---------------------------------------------------
		} else if ((oSF1Type == typeid(CallIfTrue)) || (oSF2Type == typeid(CallIfTrue))) {
			// Or(True, Op2) = True,  Or(Op1, True) = True
			return CallIfTrue::getInstance(); //--------------------------------
		}
	} else if (oCallIfType == typeid(CallIfNot)) {
		auto p0NotF = static_cast<CallIfNot*>(refCallIf.get());
		auto refSF = simplify(p0NotF->getCallIf(), oEventClass);
		const CallIf& oSF = *refSF;
		const std::type_info& oSFType = typeid(oSF);
		if (oSFType == typeid(CallIfFalse)) {
			// Not(False) = True
			return CallIfTrue::getInstance(); //--------------------------------
		} else if (oSFType == typeid(CallIfTrue)) {
			// Not(True) = False
			return CallIfFalse::getInstance(); //-------------------------------
		} else if (oSFType == typeid(CallIfNot)) {
			// Not(Not(Op)) = Op
			auto p0NNF = static_cast<CallIfNot*>(refSF.get());
			return p0NNF->getCallIf(); //---------------------------------------
		}
	}
	// cannot simplify
	return refCallIf;
}

} // namespace CallIfSimplifier

} // namespace stmi
