package de.uhoh.for1695;

import org.apache.jena.rdf.model.*;
import org.apache.jena.riot.*;
import org.apache.jena.shared.*;
import org.apache.jena.query.*;
import java.io.*;
import java.util.*;


class XnRDFmanager {
	Model model;
	void readFiles (List<String> inputFileList, String path, String fileExt) {
	    // create an empty model
        model = ModelFactory.createDefaultModel();

		// read the RDF/ttl files
		try {
			for (String inputFileName : inputFileList) {
				model.read(path + inputFileName +  fileExt, "TURTLE");
			}
		} catch (JenaException e) {
			System.out.print("Error in reading ttl files: " + e.getMessage());
		}
	}
	void writeModelAsTurtle () {
		try {
			// write it to standard out
			model.write(System.out, "TURTLE");
		} catch (JenaException e) {
			System.out.print("Error in writing imported ttl output: " + e.getMessage());
		}
	}
	void writeModelAsXML () {
		try {
			// write it to standard out
			model.write(System.out);
		} catch (JenaException e) {
			System.out.print("Error in writing imported ttl output: " + e.getMessage());
		}
	}
	ResultSet query (String queryString) {
		ResultSet results = null;
		try {
			// write it to standard out
			Query q = QueryFactory.create(queryString);
			QueryExecution qexec = QueryExecutionFactory.create(q, model) ;
			results = qexec.execSelect() ;
				
		} catch (JenaException e) {
			System.out.print("Error when running SPARQL query: " + e.getMessage());
			e.printStackTrace();
		}
		return results;
	}
	
}
class TestRDFmanager {
	
	static String prefixes = 
			  "prefix rr: <http://www.w3.org/ns/r2rml#>\n"
			+ "prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> \n"
			+ "prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> \n"
			+ "prefix xsd: <http://www.w3.org/2001/XMLSchema#> \n"
			+ "prefix jdbc: <http://d2rq.org/terms/jdbc/> \n"
			+ "prefix skos: <http://www.w3.org/2004/02/skos/core#> \n"
			+ "prefix agrovoc: <http://aims.fao.org/aos/agrovoc/> \n"
			+ "prefix dbo: <http://dbpedia.org/resource/classes#> \n"
			+ "prefix dcterms: <http://purl.org/dc/terms/> \n"
			+ "prefix owl: <http://www.w3.org/2002/07/owl#> \n"
			+ "prefix qudt: <http://qudt.org/schema/qudt/> \n"
			+ "prefix unit: <http://qudt.org/vocab/unit#> \n"
			+ "prefix agro-ont: <http://aims.fao.org/aos/agrontology#> \n"
			+ "prefix ktbl: <http://srv.ktbl.de/vocabulary#>\n"
			+ "prefix ktbl-plant: <http://srv.ktbl.de/vocabulary/plant#>\n"
			+ "prefix fa: <https://srv.ktbl.de/ontologies/FarmActivity#> \n"
			+ "prefix simlearn: <http://simlearn.opendfki.de/simlearn/core#>\n"
			+ "\n";

	
	public static void main(String[] args) {
		XnRDFmanager testRDF = new XnRDFmanager();
//		ArrayList<String> filenameList = new ArrayList<String> (Arrays.asList("output_simlearn_bodenproben","output_simlearn_arbeitsort_aftercared", "output_simlearn_masnahmen_aftercared", "output_plant_hierarchie_aftercared", "output_ktbl_vocabulary_units_aftercared","output_ktbl_vocabulary_properties_aftercared"));
		ArrayList<String> filenameList = new ArrayList<String> (Arrays.asList(
				"FarmActivity",
				"output_ktbl_vocabulary_properties_aftercared",
				"output_ktbl_vocabulary_units_aftercared",
				"output_maschinen_hierarchie_aftercared",
				"output_plant_hierarchie_aftercared",
				"r2rml_farmactivity-output",
				"r2rml_farmactivity_plant_maschine_link_output",
				"r2rml-Maschinen-output",
				"SoilTexture"
				));
		testRDF.readFiles( filenameList, "/home/troost/bwSyncShare/Simlearn/DataLI/simlearn-main-ontology/ontology/", ".ttl" );	
		//testRDF.writeModelAsXML();
/*		
		String q1 = "SELECT DISTINCT ?x "
				+ "			WHERE {"
				+ "				?x rdf:type ktbl:Plot"
				+ "			}"
				;
		ResultSet r1 = testRDF.query(prefixes +q1);
		ResultSetFormatter.outputAsCSV(System.out, r1);
		//ResultSetFormatter.outputAsJSON(System.out,  r1);
		
		
		
		String q2=" SELECT DISTINCT?label ?CropLabel ?Year ?Flaeche ?Einheit ?Yield ?YieldUnit\n"
				+ "\n"
				+ "WHERE {\n"
				+ "\n"
				+ "?x rdf:type ktbl:Plot .\n"
				+ "#?x ?p ?o .\n"
				+ "?x rdfs:label ?label  .\n"
				+ "?x ktbl:hasFieldParameter ?Parameter .\n"
				+ "  ?Parameter dcterms:date ?Year .\n"
				+ "  ?Parameter ktbl:hasCrop ?Crop .\n"
				+ "    FILTER (lang(?CropLabel) = 'de') .\n"
				+ "  ?Parameter ktbl:plotSize ?FlaecheDummy .\n"
				+ "  ?Parameter ktbl:hasYield ?YieldDummy .\n"
				+ "  ?YieldDummy qudt:value ?Yield .\n"
				+ "  ?YieldDummy qudt:unit ?YieldUnit .\n"
				+ "  ?FlaecheDummy qudt:value ?Flaeche .\n"
				+ "\n"
				+ "  ?FlaecheDummy qudt:unit ?Einheit .\n"
				+ "  ?CropX rdf:type ?Crop .\n"
				+ "\n"
				+ "  ?CropX ktbl:xnRelevant \"TRUE\"^^xsd:boolean .\n"
				+ "  ?Crop rdfs:label ?CropLabel .\n"
				+ "  \n"
				+ " ?x skos:closeMatch ?Boden .\n"
				+ "\n"
				+ "\n"
				+ "}\n"
				+ "LIMIT 500"
				;
		
		ResultSet r2 = testRDF.query(prefixes +q2);
		ResultSetFormatter.outputAsCSV(System.out, r2);
*/		
		/*String q3 = "SELECT DISTINCT?label ?CropLabel ?Year ?Flaeche ?Einheit ?Yield ?YieldUnit\n"
				+ "\n"
				+ "WHERE {\n"
				+ "\n"
				+ "?x rdf:type ktbl:Plot .\n"
				
				+ "?x rdfs:label ?label  .\n"
				+ "FILTER(CONTAINS(?label, 'Schlag 4')) .\n"
				+ "?x ktbl:hasFieldParameter ?Parameter .\n"
				+ "  ?Parameter dcterms:date ?Year .\n"
				+ "FILTER(YEAR(?Year)IN (2019,2020)) .\n"

				+ "  ?Parameter ktbl:hasCrop ?Crop .\n"
				+ "    FILTER (lang(?CropLabel) = 'de') .\n"
				+ "    FILTER (CONTAINS(?CropLabel, 'Mais')) .\n"
				+ "  ?Parameter ktbl:plotSize ?FlaecheDummy .\n"
				+ "  ?Parameter ktbl:hasYield ?YieldDummy .\n"
				+ "  ?YieldDummy qudt:value ?Yield .\n"
				+ "  ?YieldDummy qudt:unit ?YieldUnit .\n"
				+ "  ?FlaecheDummy qudt:value ?Flaeche .\n"
				+ "\n"
				+ "  ?FlaecheDummy qudt:unit ?Einheit .\n"
				+ "  ?CropX rdf:type ?Crop .\n"
				+ "\n"
				+ "  ?CropX ktbl:xnRelevant \"TRUE\"^^xsd:boolean .\n"
				+ "  ?Crop rdfs:label ?CropLabel .\n"
				+ "  \n"
				+ " ?x skos:closeMatch ?Boden . "
				+ "}"
				;
		ResultSet r3 = testRDF.query(prefixes +q3);
		ResultSetFormatter.outputAsCSV(System.out, r3);*/
		
		
		String q4 = "SELECT DISTINCT ?LabelFaGroup\n"
				+ "\n"
				+ "\n"
				+ "WHERE {\n"
				+ "?a1 a ktbl:FaGroup .\n"
				+ "?a1 rdfs:label ?LabelFaGroup .\n"
				+ "FILTER(LANG(?LabelFaGroup) = \"de\")."
				+ "  \n"
				+ "}"
				+ "";				;
		ResultSet r4 = testRDF.query(prefixes +q4);
		ResultSetFormatter.outputAsCSV(System.out, r4);
	}
}