//------------------------------------------------------------------------------
//
//	Thomas Berger (main author), Pepijn Schreinemachers, and Thorsten Arnold
//
//
//	Hohenheim University (490d)
//	Professorship for Land Use Economics in the Tropics and Subtropics
//
//	May not be distributed without permission by main author
//
//	File:				BasicEnums.h (formerly enum_defs.h)
//
//	Contents:		Contains enumeration lists
//
//
//
//------------------------------------------------------------------------------


#ifndef	_BasicEnums_h
#define	_BasicEnums_h

///assigment of water rights to agents
enum WaterAssignmentType
{	onlyActual,
	onlyRandom,
	bothRights
};

// Note: Each irrigation method requires a string BasicData::getStringIrrigationMethods(typeIrrigationMethod)
enum typeIrrigationMethod
{	irrigMethodFlood,
	irrigMethodFurrow,
	irrigMethodTerrassing,
	irrigMethodDrip,
	irrigMethodImprovedFurrow,
	irrigMethodAdvancedFurrow,//"californiano"
	irrigMethodSprinkler,
	irrigMethodTape,
	irrigMethodCenterPivot,
	irrigMethodMicroSprinkler,
	irrigMethodEND
};

// PASSED AS ARGUMENT TO MPMAS (-R## with TYPE)
// Currently, there are two "conversion types":
//    - Type 1: The conversion from MPMAS input (Activity-Key / LP-block on hydrology   --> Irrigation Table )
//    - Type 2: Conversion (WASIM abstractions / files as read from Disk  --> Sector inflows
enum enumIrrigationFactorType {
   typeIrrig_cropWat2WasimIrrigationTable,
   typeIrrig_externalInflow2LiterPerSecond,
   typeIrrig_cropWatGross2LiterPerSecond,
   typeIrrig_cropWatExpected2LiterPerSecond,
   typeIrrig_activityKey2LiterPerSecond,
   typeIrrig_activityKey2mmPerDay,
   typeIrrig_CropKey2mmPerDay, // not used
   typeIrrig_MAX,
   typeIrrig_ErrorOutput
} ;

enum TypeRoleInCoupling
{	typeSequencer,
	typeDatamanager,
	typeMPMAS,
	typeWasimWrapper,
	typeCropModel
};


enum TypeDataExchangeType
{	typeMatrixDouble,
	typeRaster2D
};


enum TypeDataExchangeChannel
{	typeControllerToPerif1, typePerif1ToController, typeControllerToPerif2, typePerif2ToController, typeControllerToPerif3, typePerif3ToController,
	typeControllerToPerif4, typePerif4ToController, typeControllerToPerif5, typePerif5ToController, typeControllerToPerif6, typePerif6ToController
};

enum TypeHydroModel
{	typeHydroNull,
	typeHydroEdic,
	typeHydroWasim,
	typeHydroExternal //formerly typeHydroLucia, now used for external models in general
};

enum TypeCropModel
{	typeCropNull,
	typeCropTspc,
	typeCropWat,
	typeCropLucia //or any other external crop-growth module
};

enum TypeEdicFlow
{	enumMethodPlantuptake,
	enumMethodSurfaceOvernight,// These are set afterwards, by hand
	enumMethodSurface,
	enumMethodLateral,
	enumMethodDeep,
	enumMethod_MAX_read,// These are automatically readfrom file
	enumMethodCanalDeep,
	enumMethodCanalLateral,
	enumMethodCanalSurface,
	enumMethodEND
};

enum enumTypeCanalModel
{	typeModeInherited,				//0
	typeModeRedistributed,			//1
	typeModeRedistributedLambda,	//2
	typeModeMixedInhRedistr,		//3
	typeModeMixedInhRedistrLambda,//4
//	typeModeMixedRedistrLambda,	//5
//	typeModeMixedInhRedistrLambda,//6
	max_typeModeMixed					//MAX
};

enum POSITIONMATRIXtranslation
{	posM_LPcol, posM_crA, posM_CropID, posM_tech, posM_max,
	posM_LUwasim, posM_freq, posM_binQuant, posM_bin, posM_numG
};

// if
//	FileTypeEnum fileTypeGisInput:	file in	input/gis/
//	FileTypeEnum fileTypeIsDAT:	file in	input/dat/
//	...see function getFullMpmasFilename
enum	EnumFileType
{	fileTypeGisInput, fileTypeGisInputYearly, fileTypeGisOutputYearly, fileTypeGisOutputMonthly,
	fileTypeIsDAT, fileTypeIsDatByCatchment, fileTypeIsDatYearly, fileTypeIsDatMonthly, fileTypeIsDatForMilp,
	fileTypeIsTEST, fileTypeIsTEST_monthly, fileTypeIsOUT, fileTypeIsOutByCatchment, fileTypeIsDAT_coupl,
	fileTypeIsOutByCatchmentAndMonth,
// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	fileTypeGisInputUserDefined,
// Troost **********end block insert**********
	fileTypePathActPrecipitation, // Tbe 150212
	fileTypePathPotEvapotranspiration,
	fileTypePathActPrices,
	fileTypeUserdefinedGisOutputYearly,
	fileTypeExtraCropActAttributeOutput
};

enum Content
{	///Gis input files
	contFarmID = 0,
	contAgentMapID,

	contSoilID, contCluID, contNetwID, contPopID,
	contMapColor, contDistFromHome, contUserMapID, contSectorID,


	contENDBASIC,

	///Special input files used by TSPC model
	contSoilN, contSoilP, contSoilK, contSoilA, contSoilC, contSoilSl, contSoilSe,
	contSlopeLen, contSlopeErosion, contSoilCond, contENDTSPC,

	///Irrigation data handled in MPMAS
	contCropAct, contPrecip, cont_SectorReuseFactor,


	///Further data handled in "coupling" matrices
	// Note: the order is important: They are allocated into an array of dimension
	// ((contENDCOUPL - contENDIRRIG + 1) = contENDCOUPL - contENDIRRIG),
	// and stored in matrices within the class cell. Used also during coupling!
	contENDIRRIG,
	contSoilMoist, contSoilMoist05, contSoilMoist1, contETpot, contETreal,
	contSurfaceRunoff, contPercol, contBaseFlow, contGroundwaterRecharge,
	contIrrigGrid, contLUGrid, contLUwasim, contETdiff, contFlowMat,
	contWATERWAYS, contDEM, contIrrigQuant, contInterceptEvap, contThrougfall,
	contENDCOUPL,
	//Note: also here the contENDCOUPL are relevant
	//monthly values are recognized as such if they lie between contENDCOUL and contENDMONTHLY
	///Monthly values, passed directly from cropMixClass to cell and is stored as 'matrixKr (1,12)' there for each month
	cont_etrToEtPot_monthly, contIrrigQuantEdic, contENDMONTHLY,

	///Crop yields,
	contCropYields = 100,
	contCrpMixClassIndex, // Index into CrpMixClass
	contUserDefinedGISInput,
	contParcelGroup,
	contRentalPayment,
	contStoverYields,
	contPreviousCropAct,
	contPreviousNRU,
	contDirectionFromHome,
	contNRU,
	contExtraCropActAttribute,
	contCommonPropertyGroup,
	contCumulGrazingPressure,
	//add new spatial content here !!!, not above if possible (exceptions for monthly/coupling content, otherwise -A flag numbers will be changed, and that would not be nice
	contLastSpatial,
	///Other formats, or special types, that are not gridded data
	contLUTable = 1000, contIrrigTable, contENDCOUPLTABLE,
	//Non-spatial input files
	fileTypeBasicData = 1100, fileTypeMarket, fileTypePCrops, fileTypeSoils, fileTypeMILP,
	fileTypeLivestock, fileTypeSegment, fileTypePopulation, fileTypeCluster,
	fileTypeEdicCalibrationFactor, fileTypeEdicSurfaceRunoff, fileTypeEdicLateralFlows, fileTypeEdicRiverFlows,
	fileTypeEdicIrrigationMethods, fileTypeRegion, fileTypeCropWat, fileTypeCropLucia, fileTypeCropSimilarity, fileTypeCropYieldTable,
	fileTypeEffectivePrecip, fileTypeActualPrecip, fileTypePotentialETr, fileTypeFloodFactor,
	fileTypeSectorRandomWaterRights, fileTypeSectorActualWaterRights, fileTypeSectorInfrastructureProjects, fileTypeSectorExistingRentalContracts,
	fileTypeLanduseByCatchment, fileTypeTimeHandler, fileTypeIrrigationFactors, fileTypeSpatialExportInfo,
	fileTypeSoilToNRU,fileTypeLandmarketParameters,fileTypeInvestmentsAttachedToLand,
	fileTypeLivSimAssetTransformationInfo,fileTypeExternalScriptCalls,fileTypeExtraAgentCharInfo,fileTypeExtraAgentCharInit,
	fileTypeAdvancedExpectationParameters,fileTypeCommonPropertyGroups,
	fileTypeENDFILETYPE,

	///Misc.
	contIsInit, contNotInit, contNotFound, contData, contNOTYPE, contEMPTY, contNoDataVal, contmapData,

	///Control files
	contCntrlData, contSectorCntrl,	contOutputData,	contMonthlyData,


	contMAXLEN
};

enum ExportType
{	expFlowMat, expIrrigTable, expLUTable, expIrrigGrid, expLUGrid,
	expWATERWAYS, expDEM, expSoilMoist,   expSoilMoist05, expSoilMoist1, expETpot, expETreal,
	expETdiff, expPrecip, expIrrigQuant, expSurfaceRunoff, expBaseFlow, expGroundwaterRecharge,
	expInterceptEvap, expThroughfall, expPercol, expNOTYPE
};


enum enumIrrigationPosition
{	posIrrID, posIrrTech, posIrrClust, posIrrLUC
};



/**
*	The enumeration SolverType is used in the solver choosing section of MipHandler::switchSolver()
*/
///	Encoding of	MIP solver type
enum SolverType 
{	osl, // ## Tbe 150116 changed order
	symphony,
	cbc,
	cplex,
	solverNone
};



///Enumeration to	encode type	of payments	in income statement
enum PaymentType
{	typeOther,
	typeEarningsInCash,
	typeEarningsInKind,
	typePurchasedFood,
	typeConsumedProducedFood,
	typeProductionExpenditure,
	typeAppreciation,
	typeDisutilities
};

///type of production cost
enum enumTypeCostProduction
{	typeCostAnnualActivity,
	typeCostPerennialActivity,
	typeCostOtherActivity,
	typeCostMAX
};

///irrigation type (former LP activity type)
enum enumIrrigationType
{	typeOtherActivity = -1,
	typeSellingBuying,				//0 = selling/buying
	typeIrrigatedAnnual,				//1 = irrigated annual
	typeIrrigatedPerennial,			//2 = irrigated perennial
	typeIrrigatedDivisibleInv,		//3 = divisible investment good with irrigation
	typeInvestmentPerennial,		//4 ## Tbe 130328
	typeRainfedAnnual,				//5 = annual rainfed crops
	typeRainfedPerennial,			//6 = perennial rainfed crops
	typeIrrigatedIndivisibleInv	//7, formerly 4 = indivisible investment good with irrigation
};

///learning parameter for adaptive expectations
enum typeLambda
{	lambdaWaterSupply,
	lambdaWaterDemand,
	lambdaMarketPrices,
	lambdaYieldCropWat,
	lambdaYieldTspc,
	lambdaMAX
};

///crop-water related info
enum enumYieldInfo
{	typeYieldInfo_waterActivityIdx,
	typeYieldInfo_areaPlanted,
	typeYieldInfo_lpColumn,
	typeYieldInfo_priceColumn,
	typeYieldInfo_yieldRow,
	typeYieldInfo_irrigationPriority,
	typeYieldInfo_irrigationType,
	typeYieldInfo_plantShare,
	typeYieldInfo_preHarvestCostsPerHa,
	typeYieldInfo_potentialYieldPerHa,
	typeYieldInfo_expectedYieldPerHa,
	typeYieldInfo_actualYieldPerHa,
	typeYieldInfo_potentialTotalProduction,
	typeYieldInfo_expectedTotalProduction,
	typeYieldInfo_actualTotalProduction,
	typeYieldInfo_reductionCoefficientKr,
	typeYieldInfo_yieldReductionFactor,
	typeYieldInfo_totalProductionLoss,//here: compared to expected
	typeYieldInfo_relativeYieldFactor,//yield as compared to "full production", needed for perennials
	typeYieldInfo_MAX
};

enum enumActivityType
{	type_sellingBuying,		//(0) Selling/buying activity
	type_irrigatedAnnual,	//(1) Growing an irrigated annual crop
	type_irrigatedPerennial,//(2) Growing an irrigated permanent crop
	type_divisibleIrrigated,//(3) Divisible investment goods with irrigation
	type_indivisible,			//(4) Indivisible investment goods without irrigation
	type_rainfedAnnual,		//(5) Rainfed annual crop
	type_rainfedPerennial,	//(6) Rainfed perennial crop
	type_activityTypeMAX		//(-1) All other activities
};

enum columnsInPermamentCropsRegistry			//<dkult_reg>
{	typePermCrops_investmentObjectID,			//formerly 17 //order may not be changed
	typePermCrops_status,                     //formerly 0  //order may not be changed

	typePermCrops_activityLP,	//quick fix ## 080812
	typePermCrops_yieldRowLP,	//quick fix ## 080812
	typePermCrops_sizeHa,		//quick fix ## 080812

	typePermCrops_currentCropYield,
	typePermCrops_currentVariableCosts,
	typePermCrops_currentPreHarvestCosts,
	typePermCrops_currentHarvestCosts,

	typePermCrops_futureCropYield,
	typePermCrops_futureVariableCosts,
	typePermCrops_futurePreHarvestCosts,
	typePermCrops_futureHarvestCosts,

	//Caution: hard-coded value for AGENTF_ENTRIES_BEFORE_LABOR

	typePermCrops_MAX 
};

enum rowsInPerennialCropsHandler					//<anbaudaten>
{	permCropsHandler_relativeYield,				//0
	permCropsHandler_preHarvestCosts,			//1
	permCropsHandler_harvestCosts,				//2
	permCropsHandler_MAX
};

enum objectTypesInLottery
{	objTypeMIN = -9,
	objTypeMarketAccess = -8,
	objTypeTimePreference = -7,
	objTypeRandomLogit = -6,
	objTypeLeverage = -5,	// -5
	objTypeLiquidity,			//	-4
	objTypeExpectations,		// -3
	objTypeInnovativeness,	// -2
	objTypeFemaleHead,		// -1
	objTypeDefault,			//  0	object with land demand
	objTypeNoLandDemand,		//	 1 object without land demand
	objTypeSymbolicObjects,	//  2
//----------------------------------------------------------
	objTypeFruitProduction,	//  3 country-specific Chile
	objTypeBerryProduction, //  4
	objTypeHiringPeakLabor,	//	 5
	objTypeMachineryT0,		//  6
	objTypeMachineryT1,		//  7
	objTypeMachineryT2,		//  8
	objTypeMachineryT3,		//  9
	objTypeHiringT2,			// 10
	objTypeHiringT3,			// 11
	objTypeFruitWithDrip,				//fruit tree to be combined with drip irrigation (see below)
	objTypeFruitWithMicrosprinkler,
	objTypeFruitWithCaliforniano,
	objTypeBerryWithDrip,
	objTypeBerryWithTape,
	objTypeTapePerennialsHeadUnit,//irrigation device that has to be combined with fruit (see above)
	objTypeTapePerennialsPlotUnit,
	objTypeDripPerennialsHeadUnit,
	objTypeDripPerennialsPlotUnit,
	objTypeMicrosprinklerPerennialsHeadUnit,
	objTypeMicrosprinklerPerennialsPlotUnit,
	objTypeCalifornianoPerennialsHeadUnit,
	objTypeCalifornianoPerennialsPlotUnit,
	objTypeIrrigationPerennialsAfterLottery,//irrigation device for perennials to be assigned after lottery (e.g. CNR projects)
	objTypeBasicImplements, //basic machinery implements such as plough, trailer etc.
	objTypeMAX
};

enum luciaCropTypes
{	typeDeactivated = -1,
	typeSeasonal,
	typeAnnual,
	typePerennial,
	typeZeroLabor,
	typeInvestment,
	typeCropTypeMAX
};


enum tradablesPriceGroupTypes
{	typePriceGroupOthers,
	typeOutputsSoldOnMarket,
	typeInputsBoughtOnMarket,
	typePurchasedFoodItems,
	typePurchasedLuxuries,
	typeOwnConsumption,
	typePriceGroupMAX
};


enum agentSurveyTypes
{	typeFarmProductionSurvey,
	typeFarmIncomeSurvey
};

enum enumCoupling
{
	withLUCIA,
	withExpertN,
	noCoupling // Troost 151026: added for completeness
};

enum agentTypes
{	agentLandowner,
	agentFarmHousehold,
	agentOtherFarm,
	agentProducerOrganization,
	agentUserAssocation,
	agentTypesMAX
};

enum landModelTypes
{	landNoTransfer,
	landToOtherFarms,
	landRentalMarket,
	landLongTermMarket,
	landLongTermParcelsMarket,
	landDistributionOnly,
	landRentalEthiopia,
	landModelTypesMAX
};

enum lotteryInputTypes
{	lotteryDefault,
	lotteryFromFile,
	assetsFromFileWithAges
};

enum specialCountryModelTypes
{	countryModeldefault,
	countryModelChile,
	countryModelUganda,
	countryModelGhana,
	countryPepijnDiss,
	countryModelGermany,
	countryModelPeru,
	countryModelTeaching
};


enum subversionModuleTypes
{	subversionDefault,
	subversionEdicChile,
	subversionFloodFactor,
	subversionExtYieldsNoMaps,
	subversionLookUpTables,
	subversionExtYieldsMaps,
	subversionExtYieldsTCPMaps,
	subversionExtYieldsLivestockMaps,
	subversionExtYieldsLivestockTCPMaps
};


enum milpModeTypes
{	modeInvestment,
	modeProduction,
	modeActualYieldsAndPrices,//for intermediate products or consumption decision
	modeActualYieldsExpectedPrices,//for marketing decisions
	modeMultiperiodStartOfYear
};

enum plotStatusTypes
{	plotFree,
	plotFarmstead,
	plotPermanentCrop,
	placeholderInList,
	plotRented
};

enum typeDecisionVariable
{	varContinuous,
	varInteger,
	varQuadraticContinuous,
	varQuadraticInteger
};

// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
enum demographyModelTypes
{
	demographyModelDefault,
	demographyModelTroost,
	demographyModelTroostWithLp
};
enum careerEvent
{
	careerEventDeath,
	careerEventBirth,
	careerEventMarriage,
	careerEventLeaving,
	careerEventRetire,
	careerEventLeavingWithMother,
	careerEventLeavingWithFather,
	careerEventSwitch
};
enum reasonNewCareer
{
	careerDescendant,
	careerMarriage,
	careerPartner,
	careerHHead,
	careerPartnerHHead,
	careerRetire,
	careerAsNewMother,
	careerAfterGenericSwitch
};
enum reasonHHeadSuccesion
{	//if updating enter before initHHead
	// and update NUMREASONSSUCCESSION in PopulationClusterList.h
	// and check consistency of input routine in PopulationClusterList.cpp
	successionAtDeath,
	successionVoluntaryRetirement,
	successionObligatoryRetirement,
	initHHead
};
// Troost **********end block insert**********

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
enum typeNRUassignment
{
	dynamicNRUassignment, staticNRUassignment, userdefinedNRUassignment
};
// Troost **********end block insert**********
enum typeofUserdefinedNRUassignment
{	udefNRUnone	= -1,
	udefNRUbySimilarity = 0,
	udefNRUbyDistance,
	udefNRUbyDistanceAndParcelSize,
	udefNRUbyDistanceDirectionAndDynamicValues
};



enum transactionTypeRentalMarket
{
	transactionTypeRMreturn,
	transactionTypeRMrent,
	transactionTypeRMsellAgentL,
	transactionTypeRMsellAgentO,
	transactionTypeRMsellAgentF
};
enum transferTypeAgentAgentPayment
{
	agentagentPaymentLandRent,
	agentagentPaymentAssetSale
};
enum agentAttributeForLp
{

	agentAttributeAgeHHhead = 1,
	agentAttributeAgeSuccessor,
	agentAttributeIncome,
	agentAttributeUserMarkHHhead,
	agentAttributeUserMarkSuccessor,
	agentAttributeUserDefHHmemberChar

};
enum householdMemberSwitchFromLp
{
	householdMemberDecisionMin = -7,
	householdMemberDecisionDie,
	householdMemberDecisionLeave,
	householdMemberDecisionRetire,
	householdMemberDecisionBecomeHead,
	householdMemberDecisionMarry,
	householdMemberDecisionGiveBirth 	= -1
};
enum householdMemberAttributesToLp
{
	householdMemberAttributeAge = 0,
	householdMemberAttributeLaborGroup,
	householdMemberAttributeLaborProvision,
	householdMemberAttributeLeave,
	householdMemberAttributeMarry,
	householdMemberAttributeRetire,
	householdMemberAttributePriorityHH,
	householdMemberAttributeMinCons,
	householdMemberAttributeOppWage,
	householdMemberAttributeLeaveWithMother,
	householdMemberAttributeLeaveWithFather,
	householdMemberAttributeFirstNutrient
};

enum landAndAssetSplits
{
	landAndAssetSplitsCash = 0,
	landAndAssetSplitsLand,
	landAndAssetSplitsAsset,
	landAndAssetSplitsMax
};

enum doInvestModes
{
	lpInvestDontStoreSolution,
	lpInvestNormalInvestment,
	lpInvestPrelimLandApplication,
	lpInvestPrelimExternalScript
};

#endif

