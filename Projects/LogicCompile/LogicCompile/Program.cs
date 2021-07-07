using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace LogicCompile
{
	class Program
	{
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

		static List<sTokenDef_t> caSectionDefs;
		static List<sTokenDef_t> caDefinitionDefs;
		static List<sTokenDef_t> caCommandDefs;
		static List<sTokenDef_t> caParamDefs;
		static List<sLine_t> caSrcLines;

		static string cstrTokenXMLFile = "Tokens.xml";

		static void Main(string[] args) {
			string strSourceFile;

			caSectionDefs = new List<sTokenDef_t>();
			caDefinitionDefs = new List<sTokenDef_t>();
			caCommandDefs = new List<sTokenDef_t>();
			caParamDefs = new List<sTokenDef_t>();

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

			//Tokenizer

			//For each program...
			//Break into sections

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

					for(int nCtr = 0; nCtr < NewToken.aParamNames.Count; nCtr++) {
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
	}
}
