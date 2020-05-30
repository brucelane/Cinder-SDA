#include "VDSocketio.h"

using namespace videodromm;

VDSocketio::VDSocketio(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;

	CI_LOG_V("VDSocketio constructor");
	// SocketIO
	clientConnected = false;
	
	if (mVDSettings->mAreSocketIOEnabledAtStartup) sioConnect();
	mPingTime = getElapsedSeconds();

}

void VDSocketio::updateParams(int iarg0, float farg1) {
	if (farg1 > 0.1) {
		//avoid to send twice
		if (iarg0 == 61) {
			// right arrow
			mVDSettings->iBlendmode--;
			if (mVDSettings->iBlendmode < 0) mVDSettings->iBlendmode = mVDAnimation->getBlendModesCount() - 1;
		}
		if (iarg0 == 62) {
			// left arrow
			mVDSettings->iBlendmode++;
			if (mVDSettings->iBlendmode > mVDAnimation->getBlendModesCount() - 1) mVDSettings->iBlendmode = 0;
		}
	}
	if (iarg0 > 0 && iarg0 < 9) {
		// sliders 
		mVDAnimation->setUniformValue(iarg0, farg1);
	}
	if (iarg0 > 10 && iarg0 < 19) {
		// rotary 
		mVDAnimation->setUniformValue(iarg0, farg1);
		// audio multfactor
		if (iarg0 == mVDSettings->IAUDIOX) mVDAnimation->setUniformValue(iarg0, (farg1 + 0.01) * 10);
		// exposure
		if (iarg0 == mVDSettings->IEXPOSURE) mVDAnimation->setUniformValue(iarg0, (farg1 + 0.01) * mVDAnimation->getMaxUniformValueByIndex(iarg0));

		sioWrite("{\"params\" :[{\"name\":" + toString(iarg0) + ",\"value\":" + toString(mVDAnimation->getUniformValue(iarg0)) + "}]}");

	}
	// buttons
	if (iarg0 > 20 && iarg0 < 29) {
		// select index
		mVDSettings->selectedWarp = iarg0 - 21;
	}
	/*if (iarg0 > 30 && iarg0 < 39)
	{
	// select input
	mVDSettings->mWarpFbos[mVDSettings->selectedWarp].textureIndex = iarg0 - 31;
	// activate
	mVDSettings->mWarpFbos[mVDSettings->selectedWarp].active = !mVDSettings->mWarpFbos[mVDSettings->selectedWarp].active;
	}*/
	if (iarg0 > 40 && iarg0 < 49) {
		// low row 
		mVDAnimation->setUniformValue(iarg0, farg1);
	}
}

string * VDSocketio::getBase64Image() { 
	streamReceived = false; 
	return &mBase64String; 
}
//dreads(1, 0, 0.8).out(o0)
void VDSocketio::parseMessage(string msg) {
	mVDSettings->mSocketIOMsg = "\nWS onRead";
	// mVDSettings->mSocketIONewMsg = true;
	if (!msg.empty()) {
		mVDSettings->mSocketIOMsg += ": " + msg.substr(0, mVDSettings->mMsgLength);
		//CI_LOG_V("sio msg: " + msg);
		string first = msg.substr(0, 1);
		if (first == "{") {
			// json
			JsonTree json;
			try {
				json = JsonTree(msg);
				// web controller
				if (json.hasChild("params")) {
					JsonTree jsonParams = json.getChild("params");
					for (JsonTree::ConstIter jsonElement = jsonParams.begin(); jsonElement != jsonParams.end(); ++jsonElement) {
						int name = jsonElement->getChild("name").getValue<int>();
						float value = jsonElement->getChild("value").getValue<float>();
						// basic name value 
						mVDAnimation->setUniformValue(name, value);
						// dispatch to clients
						if (mVDSettings->mIsSocketIOServer) {
							sioWrite(msg);
						}
					}
				}

				if (json.hasChild("k2")) {
					JsonTree jsonParams = json.getChild("k2");
					for (JsonTree::ConstIter jsonElement = jsonParams.begin(); jsonElement != jsonParams.end(); ++jsonElement) {
						int name = jsonElement->getChild("name").getValue<int>();
						string value = jsonElement->getChild("value").getValue();
						vector<string> vs = split(value, ",");
						vec4 v = vec4(strtof((vs[0]).c_str(), 0), strtof((vs[1]).c_str(), 0), strtof((vs[2]).c_str(), 0), strtof((vs[3]).c_str(), 0));
						// basic name value 
						mVDAnimation->setVec4UniformValueByIndex(name, v);
					}
				}

				if (json.hasChild("event")) {
					JsonTree jsonEvent = json.getChild("event");
					string val = jsonEvent.getValue();
					// check if message exists
					if (json.hasChild("message")) {
						if (val == "canvas") {
							// we received a jpeg base64
							mBase64String = json.getChild("message").getValue<string>();
							streamReceived = true;
						}
						else if (val == "params") {
							//{"event":"params","message":"{\"params\" :[{\"name\" : 12,\"value\" :0.132}]}"}
							JsonTree jsonParams = json.getChild("message");
							for (JsonTree::ConstIter jsonElement = jsonParams.begin(); jsonElement != jsonParams.end(); ++jsonElement) {
								int name = jsonElement->getChild("name").getValue<int>();
								float value = jsonElement->getChild("value").getValue<float>();
								CI_LOG_V("VDSocketio jsonParams.mValue:" + toString(name));
								// basic name value 
								mVDAnimation->setUniformValue(name, value);
							}
						}
						else if (val == "hydra") {
							receivedUniformsString = json.getChild("message").getValue<string>();
							shaderUniforms = true;
							// force to display
							mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOA, 0);
							mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOB, 1);
						}
						else if (val == "frag") {
							// we received a fragment shader string
							receivedFragString = json.getChild("message").getValue<string>();
							shaderReceived = true;
							// force to display
							mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOA, 0);
							mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOB, 1);
						}
						else {
							// we received a fragment shader string
							CI_LOG_V("VDSocketio unknown event: " + val);
						}
						//string evt = json.getChild("event").getValue<string>();
					}
				}
				if (json.hasChild("Data")) {
					JsonTree jsonData = json.getChild("Data");
					if (jsonData.hasChild("Code")) {
						JsonTree jsonCode = jsonData.getChild("Code");
						receivedFragString = jsonCode.getValue<string>();
						shaderReceived = true;
					}
				}
				if (json.hasChild("cmd")) {
					JsonTree jsonCmd = json.getChild("cmd");
					for (JsonTree::ConstIter jsonElement = jsonCmd.begin(); jsonElement != jsonCmd.end(); ++jsonElement) {
						receivedType = jsonElement->getChild("type").getValue<int>();
						switch (receivedType)
						{
						case 0:
							// change fbo
							// react-nexusui
							// from changeWarpFboIndex
							receivedWarpIndex = jsonElement->getChild("warp").getValue<float>(); // TODO int, useless for now
							receivedFboIndex = jsonElement->getChild("fbo").getValue<float>(); // TODO int
							receivedSlot = jsonElement->getChild("slot").getValue<float>(); // TODO int
							if (receivedSlot == 0) {
								mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOA, receivedFboIndex);
							}
							else {
								mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOB, receivedFboIndex);
							}
							break;
						case 1:
							// from changeWarpShaderIndex
							//receivedWarpIndex = jsonElement->getChild("warp").getValue<float>(); // TODO int
							//receivedShaderIndex = jsonElement->getChild("shader").getValue<float>(); // TODO int
							//receivedSlot = jsonElement->getChild("slot").getValue<float>(); // TODO int
							//if (receivedSlot == 0) {
							//	mVDMix->setWarpAShaderIndex(receivedWarpIndex, receivedShaderIndex);
							//}
							//else {
							//	mVDMix->setWarpBShaderIndex(receivedWarpIndex, receivedShaderIndex);
							//}
							//break;
						case 2:
							// change tempo
							mVDAnimation->useTimeWithTempo();
							mVDAnimation->setBpm(jsonElement->getChild("tempo").getValue<float>());
							CI_LOG_I("tempo:" + toString(mVDAnimation->getBpm()));
							break;
						case 3:
							// change beat
							mVDAnimation->setUniformValue(mVDSettings->ITIME, jsonElement->getChild("beat").getValue<float>());
							CI_LOG_I("time:" + toString(mVDSettings->ITIME) + " " + toString(mVDAnimation->getUniformValue(mVDSettings->ITIME)));
							break;
						/*case 4:
							// change phase
							mVDAnimation->setUniformValue(mVDSettings->IPHASE, jsonElement->getChild("phase").getValue<float>());
							mVDAnimation->setIntUniformValueByIndex(mVDSettings->IBEAT, (int)jsonElement->getChild("phase").getValue<float>());
							CI_LOG_I("phase:" + toString(mVDSettings->IPHASE) + " " + toString(mVDAnimation->getUniformValue(mVDSettings->IPHASE))
								+ "beat:" + toString(mVDSettings->IBEAT) + " " + toString(mVDAnimation->getIntUniformValueByIndex(mVDSettings->IBEAT))
							);
							break;*/
						default:
							break;
						}

					}

				}
			}
			catch (cinder::JsonTree::Exception exception) {
				mVDSettings->mSocketIOMsg += "\n error jsonparser exception: ";
				mVDSettings->mSocketIOMsg += exception.what();
				mVDSettings->mSocketIOMsg += "  ";
			}
		}
		else if (msg.substr(0, 2) == "/*") {
			// shader with json info				
			unsigned closingCommentPosition = msg.find("*/");
			if (closingCommentPosition > 0) {
				JsonTree json;
				try {
					// create folders if they don't exist
					fs::path pathsToCheck = getAssetPath("") / "glsl";
					if (!fs::exists(pathsToCheck)) fs::create_directory(pathsToCheck);
					pathsToCheck = getAssetPath("") / "glsl" / "received";
					if (!fs::exists(pathsToCheck)) fs::create_directory(pathsToCheck);
					pathsToCheck = getAssetPath("") / "glsl" / "processed";
					if (!fs::exists(pathsToCheck)) fs::create_directory(pathsToCheck);
					// find commented header
					string jsonHeader = msg.substr(2, closingCommentPosition - 2);
					ci::JsonTree::ParseOptions parseOptions;
					parseOptions.ignoreErrors(false);
					json = JsonTree(jsonHeader, parseOptions);
					string title = json.getChild("title").getValue<string>();
					string fragFileName = title + ".frag"; // with uniforms
					string glslFileName = title + ".glsl"; // without uniforms, need to include shadertoy.inc
					string shader = msg.substr(closingCommentPosition + 2);

					string processedContent = "/*" + jsonHeader + "*/";
					// check uniforms presence
					std::size_t foundUniform = msg.find("uniform");

					if (foundUniform != std::string::npos) {
						// found uniforms
					}
					else {
						// save glsl file without uniforms as it was received
						fs::path currentFile = getAssetPath("") / "glsl" / "received" / glslFileName;
						ofstream mFrag(currentFile.string(), std::ofstream::binary);
						mFrag << msg;
						mFrag.close();
						CI_LOG_V("received file saved:" + currentFile.string());
						//mVDSettings->mShaderToLoad = currentFile.string(); 
						// uniforms not found, add include
						processedContent += "#include shadertoy.inc";
					}
					processedContent += shader;

					//mShaders->loadLiveShader(processedContent); // need uniforms declared
					// route it to SocketIO clients
					if (mVDSettings->mIsRouter) {
						sioWrite(msg);
					}

					// save processed file
					fs::path processedFile = getAssetPath("") / "glsl" / "processed" / fragFileName;
					ofstream mFragProcessed(processedFile.string(), std::ofstream::binary);
					mFragProcessed << processedContent;
					mFragProcessed.close();
					CI_LOG_V("processed file saved:" + processedFile.string());
					// USELESS? mVDSettings->mShaderToLoad = processedFile.string();
				}
				catch (cinder::JsonTree::Exception exception) {
					mVDSettings->mSocketIOMsg += "\nerror jsonparser exception: ";
					mVDSettings->mSocketIOMsg += exception.what();
					mVDSettings->mSocketIOMsg += "  ";
				}
			}
		}
		/* OBSOLETE
		else if (msg.substr(0, 7) == "uniform") {
			// fragment shader from live coding
			mVDSettings->mShaderToLoad = msg;
			// route it to SocketIO clients
			if (mVDSettings->mIsRouter) {
				sioWrite(msg);
			}
		}*/
		else if (msg.substr(0, 7) == "#version") {
			// fragment shader from live coding
			//mShaders->loadLiveShader(msg);
			// route it to SocketIO clients
			if (mVDSettings->mIsRouter) {
				sioWrite(msg);
			}
		}
		else if (first == "/")
		{
			// osc from videodromm-nodejs-router
			/*int f = msg.size();
			const char c = msg[9];
			int s = msg[12];
			int t = msg[13];
			int u = msg[14];*/
			CI_LOG_V(msg);
		}
		else if (first == "I") {

			if (msg == "ImInit") {
				// send ImInit OK
				/*if (!remoteClientActive) { remoteClientActive = true; ForceKeyFrame = true;
				// Send confirmation mServer.write("ImInit"); // Send font texture unsigned char* pixels; int width, height;
				ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&pixels, &width, &height); PreparePacketTexFont(pixels, width, height);SendPacket();
				}*/
			}
			else if (msg.substr(0, 11) == "ImMouseMove") {
				/*string trail = msg.substr(12);
				unsigned commaPosition = trail.find(",");
				if (commaPosition > 0) { mouseX = atoi(trail.substr(0, commaPosition).c_str());
				mouseY = atoi(trail.substr(commaPosition + 1).c_str()); ImGuiIO& io = ImGui::GetIO();
				io.MousePos = toPixels(vec2(mouseX, mouseY)); }*/
			}
			else if (msg.substr(0, 12) == "ImMousePress") {
				/*ImGuiIO& io = ImGui::GetIO(); // 1,0 left click 1,1 right click
				io.MouseDown[0] = false; io.MouseDown[1] = false; int rightClick = atoi(msg.substr(15).c_str());
				if (rightClick == 1) { io.MouseDown[0] = false; io.MouseDown[1] = true; }
				else { io.MouseDown[0] = true; io.MouseDown[1] = false;
				}*/
			}
		}
	}
}
string VDSocketio::getReceivedShader() {
	shaderReceived = false;
	return receivedFragString;
}
string VDSocketio::getReceivedUniforms() {
	shaderUniforms = false;
	return receivedUniformsString;
}

void VDSocketio::sioConnect() {
	clientConnected = true;
}
void VDSocketio::sioClientConnect()
{
	stringstream s;
	if (mVDSettings->mSocketIOPort == 80) {
		s << mVDSettings->mSocketIOProtocol << mVDSettings->mSocketIOHost;
	}
	else {
		if (mVDSettings->mSocketIOPort == 9000) {
			s << mVDSettings->mSocketIOProtocol << mVDSettings->mSocketIOHost << ":" << mVDSettings->mSocketIOPort << "/" << mVDSettings->mSocketIORoom << "/" << mVDSettings->mSocketIONickname;
		}
		else {
			s << mVDSettings->mSocketIOProtocol << mVDSettings->mSocketIOHost << ":" << mVDSettings->mSocketIOPort;
		}
	}
	// BL TEMP s << "https://" << mVDSettings->mSocketIOHost << ":" << mVDSettings->mSocketIOPort;
	mClient.connect(s.str());
}

void VDSocketio::sioWrite(string msg)
{
	if (mVDSettings->mAreSocketIOEnabledAtStartup)
	{
		CI_LOG_V("sioWrite send: " + msg);
		if (clientConnected) mClient.socket()->emit("test_text", sio::string_message::create(msg));
		
	}
}

void VDSocketio::sendJSON(string params) {

	sioWrite(params);

}
void VDSocketio::toggleAuto(unsigned int aIndex) {
	// toggle
	//mVDAnimation->toggleAuto(aIndex);
	// TODO send json	
}
void VDSocketio::toggleTempo(unsigned int aIndex) {
	// toggle
	//mVDAnimation->toggleTempo(aIndex);
	// TODO send json	
}
void VDSocketio::toggleValue(unsigned int aIndex) {
	// toggle
	//mVDAnimation->toggleValue(aIndex);
	stringstream sParams;
	// TODO check boolean value:
	sParams << "{\"params\" :[{\"name\" : " << aIndex << ",\"value\" : " << (int)mVDAnimation->getBoolUniformValueByIndex(aIndex) << "}]}";
	string strParams = sParams.str();
	sendJSON(strParams);
}
void VDSocketio::resetAutoAnimation(unsigned int aIndex) {
	// reset
	//mVDAnimation->resetAutoAnimation(aIndex);
	// TODO: send json	
}

void VDSocketio::changeBoolValue(unsigned int aControl, bool aValue) {
	// check if changed
	mVDAnimation->setBoolUniformValueByIndex(aControl, aValue);
	stringstream sParams;
	// TODO: check boolean value:
	sParams << "{\"params\" :[{\"name\" : " << aControl << ",\"value\" : " << (int)aValue << "}]}";
	string strParams = sParams.str();
	sendJSON(strParams);
}
void VDSocketio::changeIntValue(unsigned int aControl, int aValue) {
	mVDAnimation->setIntUniformValueByIndex(aControl, aValue);
}
void VDSocketio::changeFloatValue(unsigned int aControl, float aValue, bool forceSend, bool toggle, bool increase, bool decrease) {
	/*if (aControl == 31) {
		CI_LOG_V("old value " + toString(mVDAnimation->getUniformValue(aControl)) + " newvalue " + toString(aValue));
	}*/
	float newValue;
	if (increase) {
		// increase
		newValue = mVDAnimation->getUniformValue(aControl) + 0.1f;
		if (newValue > 1.0f) newValue = 1.0f;
		aValue = newValue;
	}
	else {
		// decrease
		if (decrease) {
			newValue = mVDAnimation->getUniformValue(aControl) - 0.1f;
			if (newValue < 0.0f) newValue = 0.0f;
			aValue = newValue;
		}
		else { 
			// toggle
			if (toggle) {
				newValue = mVDAnimation->getUniformValue(aControl);
				if (newValue > 0.0f) { newValue = 0.0f; }
				else { newValue = 1.0f; } // Check for max instead?
				aValue = newValue;
			}
		}
	}
	
	// check if changed
	if ( (mVDAnimation->setUniformValue(aControl, aValue) && aControl != mVDSettings->IFPS) || forceSend) {
		stringstream sParams;
		// update color vec3
		if (aControl > 0 && aControl < 4) {
			mVDAnimation->setVec3UniformValueByIndex(mVDSettings->ICOLOR, vec3(
				mVDAnimation->getUniformValue(mVDSettings->IFR),
				mVDAnimation->getUniformValue(mVDSettings->IFG),
				mVDAnimation->getUniformValue(mVDSettings->IFB)
			));
			
		}
		// update mouse vec4
		if (aControl > 41 && aControl < 45) {
			mVDAnimation->setVec4UniformValueByIndex(mVDSettings->IMOUSE, vec4(
				mVDAnimation->getUniformValue(mVDSettings->IMOUSEX), 
				mVDAnimation->getUniformValue(mVDSettings->IMOUSEY), 
				mVDAnimation->getUniformValue(mVDSettings->IMOUSEZ),
				1.0));
			 
		}

		// update iResolution vec3
		if (aControl == 29 || aControl ==30) {
			mVDAnimation->setVec3UniformValueByIndex(mVDSettings->IRESOLUTION, vec3(mVDAnimation->getUniformValue(mVDSettings->IRESOLUTIONX), mVDAnimation->getUniformValue(mVDSettings->IRESOLUTIONY), 1.0));
		}
		sParams << "{\"params\" :[{\"name\" : " << aControl << ",\"value\" : " << mVDAnimation->getUniformValue(aControl) << "}]}";
		string strParams = sParams.str();
		
		sendJSON(strParams);
	}
}
void VDSocketio::changeShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex, unsigned int aSlot) {
	//aSlot 0 = A, 1 = B,...
	stringstream sParams;
	sParams << "{\"cmd\" :[{\"type\" : 1,\"warp\" : " << aWarpIndex << ",\"shader\" : " << aWarpShaderIndex << ",\"slot\" : " << aSlot << "}]}";
	string strParams = sParams.str();
	sendJSON(strParams);
}
void VDSocketio::changeWarpFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex, unsigned int aSlot) {
	//aSlot 0 = A, 1 = B,...
	stringstream sParams;
	sParams << "{\"cmd\" :[{\"type\" : 0,\"warp\" : " << aWarpIndex << ",\"fbo\" : " << aWarpFboIndex << ",\"slot\" : " << aSlot << "}]}";
	string strParams = sParams.str();
	sendJSON(strParams);
}
void VDSocketio::changeFragmentShader(const string& aFragmentShaderText) {

	stringstream sParams;
	sParams << "{\"event\" : \"frag\",\"message\" : \"" << aFragmentShaderText << "\"}";
	string strParams = sParams.str();
	sendJSON(strParams);
}

void VDSocketio::update() {
}
