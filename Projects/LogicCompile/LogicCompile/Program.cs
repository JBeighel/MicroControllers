using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml;

namespace LogicCompile
{
	class Program {
		private enum eTokenType_t {
			Unknown,
			Section,
			Definition,
			Command,
			Param,
		}

		private enum eSections_t {
			Unknown,
			Program,
			GlobalVars,
			LocalVars,
			Inputs,
			Outputs,
			Code,
		}

		private struct sTokenDef_t {
			public eTokenType_t eType;
			public string strName;
			public string strRegEx;
			public UInt32 nCode;
			public List<string> aParamNames;
			public UInt32 nValGroup;
		}

		private struct sToken_t {
			public eTokenType_t eType;
			public string strName;
			public UInt32 nCode;
			public string strValue;
		}

		private struct sLine_t {
			public UInt32 nLineNum;
			public string strLine;
			public List<sToken_t> aTokens;
			public eSections_t eSection;
		}

		private struct sProgramLines_t {
			public string strName;
			public UInt32 nIndex;
			public UInt32 nNumInputs;
			public UInt32 nNumOutputs;
			public UInt32 nNumLocals;
			public List<sLine_t> aInputsLines;
			public List<sLine_t> aOutputsLines;
			public List<sLine_t> aLocalsLines;
			public List<sLine_t> aCodeLines;
		}

		static List<sTokenDef_t> caSectionDefs;
		static List<sTokenDef_t> caDefinitionDefs;
		static List<sTokenDef_t> caCommandDefs;
		static List<sTokenDef_t> caParamDefs;
		static List<sLine_t> caSrcLines;

		static Dictionary<string, sProgramLines_t> aPrograms;
		static List<sLine_t> caGlobalsLines;

		static string cstrTokenXMLFile = "Tokens.xml";

		static void Main(string[] args) {
			string strSourceFile;

			caSectionDefs = new List<sTokenDef_t>();
			caDefinitionDefs = new List<sTokenDef_t>();
			caCommandDefs = new List<sTokenDef_t>();
			caParamDefs = new List<sTokenDef_t>();
			aPrograms = new Dictionary<string, sProgramLines_t>();
			caGlobalsLines = new List<sLine_t>();

			caSrcLines = new List<sLine_t>();

			//Param gets file name (and format?)
			if (args.Length != 1) {
				Console.WriteLine("You must specify a source file");
				return;
			}
			strSourceFile = args[0];

			//Load token file
			LoadTokenDefs(cstrTokenXMLFile);

			//Load the source file into memory, break on new lines
			strSourceFile = System.IO.File.ReadAllText(strSourceFile);
			//No preprocessor without section tags for it
			//Break into Program sections (should contain all other sections)
			CreateProgramLines(strSourceFile);
			SourceToSections(caSrcLines, caGlobalsLines, aPrograms);

			//Tokenizer

			//For each program...
			//Break into sections
			//Tokenize lines that match sections
			//Use those to say what section type we're in

			//For each section...
			//Tokenize each line based on section type (stores type and text/value)
			//Counts of inputs, outputs, locals, globals, and instructions
			//Create lists of local names (includes inputs and outputs) with initial values
			//Create list of global names with initial value

			//Compiler
			//verifies token order:
			//Program section contains Code section
			//Contains 0 or 1 of Inputs, outputs, and locals

			//Var section token order
			//varname vartype* assignment
			//Assignment only valid in Local and Global var sections

			//Code section token order
			//command param
			//param must be a type in the comma separated attribute of command tag
		}

		static void LoadTokenDefs(string strDefXml) {
			XmlDocument xmlFile = new XmlDocument();
			XmlNodeList aNodes;
			sTokenDef_t NewToken;
			bool bParse;

			xmlFile.Load(strDefXml);

			//Load all section tags
			aNodes = xmlFile.SelectNodes("/tokens/section");
			foreach (XmlNode Tag in aNodes) {
				NewToken = new sTokenDef_t();

				NewToken.eType = eTokenType_t.Section;

				if (Tag.Attributes["name"] != null) {
					NewToken.strName = Tag.Attributes["name"].InnerText;
				} else {
					throw new Exception("Found section tag with no name attribute.  Unable to load token XML");
				}

				if (Tag.Attributes["test"] != null) {
					NewToken.strRegEx = Tag.Attributes["test"].InnerText;
				} else {
					throw new Exception("Found section tag with no test attribute.  Unable to load token XML");
				}

				if (Tag.Attributes["valuegroup"] != null) {
					bParse = UInt32.TryParse(Tag.Attributes["valuegroup"].InnerText, out NewToken.nValGroup);

					if (bParse == false) {
						throw new Exception("Found section tag with invalid valuegroup attribute.  Unable to load token XML");
					}
				}

				caSectionDefs.Add(NewToken);
			}

			//Load all definition tags
			aNodes = xmlFile.SelectNodes("/tokens/definition");
			foreach (XmlNode Tag in aNodes) {
				NewToken = new sTokenDef_t();

				NewToken.eType = eTokenType_t.Section;

				if (Tag.Attributes["name"] != null) {
					NewToken.strName = Tag.Attributes["name"].InnerText;
				} else {
					throw new Exception("Found definition tag with no name attribute.  Unable to load token XML");
				}

				if (Tag.Attributes["test"] != null) {
					NewToken.strRegEx = Tag.Attributes["test"].InnerText;
				} else {
					throw new Exception("Found definition tag with no test attribute.  Unable to load token XML");
				}

				if (Tag.Attributes["valuegroup"] != null) {
					bParse = UInt32.TryParse(Tag.Attributes["valuegroup"].InnerText, out NewToken.nValGroup);

					if (bParse == false) {
						throw new Exception("Found definition tag with invalid valuegroup attribute.  Unable to load token XML");
					}
				}

				caDefinitionDefs.Add(NewToken);
			}

			//Load all command tags
			aNodes = xmlFile.SelectNodes("/tokens/command");
			foreach (XmlNode Tag in aNodes) {
				NewToken = new sTokenDef_t();

				NewToken.eType = eTokenType_t.Section;

				if (Tag.Attributes["name"] != null) {
					NewToken.strName = Tag.Attributes["name"].InnerText;
				} else {
					throw new Exception("Found command tag with no name attribute.  Unable to load token XML");
				}

				if (Tag.Attributes["test"] != null) {
					NewToken.strRegEx = Tag.Attributes["test"].InnerText;
				} else {
					throw new Exception("Found command tag with no test attribute.  Unable to load token XML");
				}

				if (Tag.Attributes["param"] != null) {
					NewToken.aParamNames = new List<string>(Tag.Attributes["param"].InnerText.Split(','));

					for (int nCtr = 0; nCtr < NewToken.aParamNames.Count; nCtr++) {
						NewToken.aParamNames[nCtr] = NewToken.aParamNames[nCtr].Trim();
					}
				} else {
					throw new Exception("Found command tag with no param attribute.  Unable to load token XML");
				}

				if (Tag.Attributes["valuegroup"] != null) {
					bParse = UInt32.TryParse(Tag.Attributes["valuegroup"].InnerText, System.Globalization.NumberStyles.HexNumber, System.Globalization.CultureInfo.InvariantCulture, out NewToken.nValGroup);

					if (bParse == false) {
						throw new Exception("Found command tag with invalid valuegroup attribute.  Unable to load token XML");
					}
				}

				if (Tag.Attributes["code"] != null) {
					bParse = UInt32.TryParse(Tag.Attributes["code"].InnerText, System.Globalization.NumberStyles.HexNumber, System.Globalization.CultureInfo.InvariantCulture, out NewToken.nCode);

					if (bParse == false) {
						throw new Exception("Found command tag with invalid code attribute.  Unable to load token XML");
					}
				} else {
					throw new Exception("Found command tag with no code attribute.  Unable to load token XML");
				}

				caCommandDefs.Add(NewToken);
			}

			//Load all param tags
			aNodes = xmlFile.SelectNodes("/tokens/param");
			foreach (XmlNode Tag in aNodes) {
				NewToken = new sTokenDef_t();

				NewToken.eType = eTokenType_t.Section;

				if (Tag.Attributes["name"] != null) {
					NewToken.strName = Tag.Attributes["name"].InnerText;
				} else {
					throw new Exception("Found param tag with no name attribute.  Unable to load token XML");
				}

				if (Tag.Attributes["test"] != null) {
					NewToken.strRegEx = Tag.Attributes["test"].InnerText;
				} else {
					throw new Exception("Found param tag with no test attribute.  Unable to load token XML");
				}

				if (Tag.Attributes["valuegroup"] != null) {
					bParse = UInt32.TryParse(Tag.Attributes["valuegroup"].InnerText, out NewToken.nValGroup);

					if (bParse == false) {
						throw new Exception("Found param tag with invalid valuegroup attribute.  Unable to load token XML");
					}
				}

				if (Tag.Attributes["code"] != null) {
					bParse = UInt32.TryParse(Tag.Attributes["code"].InnerText, out NewToken.nCode);

					if (bParse == false) {
						throw new Exception("Found param tag with invalid code attribute.  Unable to load token XML");
					}
				} else {
					throw new Exception("Found param tag with no code attribute.  Unable to load token XML");
				}

				caParamDefs.Add(NewToken);
			}

			return;
		}

		static void CreateProgramLines(string strSource) {
			string[] aTxtLines = strSource.Split('\n');
			sLine_t NewLine;
			UInt32 nCtr;

			nCtr = 0;
			foreach (string strLine in aTxtLines) {
				NewLine = new sLine_t();
				nCtr += 1;

				if (String.IsNullOrWhiteSpace(strLine) == true) {
					continue;
				}

				NewLine.strLine = strLine.Trim();
				NewLine.nLineNum = nCtr;
				NewLine.aTokens = new List<sToken_t>();

				caSrcLines.Add(NewLine);
			}

			return;
		}

		static void SourceToSections(List<sLine_t> aSrcLines, List<sLine_t> aGlobals, Dictionary<string, sProgramLines_t> aPrograms) {
			int nSrcCtr, nDefCtr;
			Regex rxTest;
			Match mResult;
			string strLine;
			eSections_t eCurrSection, ePriorSection;
			sToken_t NewToken;
			sProgramLines_t NewProg = new sProgramLines_t(); ;

			eCurrSection = eSections_t.Unknown;
			ePriorSection = eSections_t.Unknown;

			for (nSrcCtr = 0; nSrcCtr < aSrcLines.Count; nSrcCtr++) {
				strLine = aSrcLines[nSrcCtr].strLine;

				//Check to see if it represents a section
				for (nDefCtr = 0; nDefCtr < caSectionDefs.Count; nDefCtr++) {
					//Add the anchor to be sure we match the start of the line
					rxTest = new Regex(@"^\s*" + caSectionDefs[nDefCtr].strRegEx);

					mResult = rxTest.Match(strLine);

					if (mResult.Success == true) {
						//Cut the matched text from the line
						strLine.Substring(mResult.Index + mResult.Length);

						//Pull out the token information and add it to the line's token list
						NewToken = new sToken_t();

						NewToken.eType = eTokenType_t.Section;
						NewToken.strName = caSectionDefs[nDefCtr].strName;
						NewToken.nCode = caSectionDefs[nDefCtr].nCode;
						if (caSectionDefs[nDefCtr].nValGroup > 0) {
							NewToken.strValue = mResult.Groups[(int)caSectionDefs[nDefCtr].nValGroup].Value;
						}
						
						aSrcLines[nSrcCtr].aTokens.Add(NewToken);

						//Lets update the section information
						switch (caSectionDefs[nDefCtr].strName) {
						case "programstart":
							if (eCurrSection != eSections_t.Unknown) {
								//Programs can't be nested in other sections
								throw new Exception(String.Format("Found program section named {0} nested inside a {1} section on line {2}", NewToken.strValue, eCurrSection, aSrcLines[nSrcCtr].nLineNum));
							}

							//Starting a new program
							eCurrSection = eSections_t.Program;
							NewProg = new sProgramLines_t();
							NewProg.aCodeLines = new List<sLine_t>();
							NewProg.aInputsLines = new List<sLine_t>();
							NewProg.aLocalsLines = new List<sLine_t>();
							NewProg.aOutputsLines = new List<sLine_t>();
							NewProg.nIndex = (UInt32)aPrograms.Count;
							NewProg.strName = NewToken.strValue;

							break;
						case "programend":
							if (eCurrSection != eSections_t.Program) {
								throw new Exception("Found end program when not in a program section on line " + aSrcLines[nSrcCtr].nLineNum);
							}

							//Close out the program and add it to the list
							aPrograms.Add(NewProg.strName, NewProg);
							eCurrSection = ePriorSection;
							ePriorSection = eSections_t.Unknown;

							break;
						case "globalliststart":
							if (eCurrSection != eSections_t.Unknown) {
								//Global lists can't be nested in other sections
								throw new Exception(String.Format("Found global list section nested inside a {0} section on line {1}", eCurrSection, aSrcLines[nSrcCtr].nLineNum));
							}

							ePriorSection = eCurrSection;
							eCurrSection = eSections_t.GlobalVars;

							break;
						case "globallistend":
							if (eCurrSection != eSections_t.GlobalVars) {
								throw new Exception("Found end global list when not in a global list section on line " + aSrcLines[nSrcCtr].nLineNum);
							}

							eCurrSection = ePriorSection;
							ePriorSection = eSections_t.Unknown;
							break;
						case "inputliststart":
						case "inputlistend":
						case "outputliststart":
						case "outputlistend":
						case "localliststart":
						case "locallistend":
						case "codestart":
						case "codeend":
						default:
							throw new Exception(String.Format("Found unrecognized section type {0} in definitions", caSectionDefs[nDefCtr].strName));
						}

						nDefCtr += caSectionDefs.Count; //Stop checking definitions
					}
				}
			}
		}
	}
}
