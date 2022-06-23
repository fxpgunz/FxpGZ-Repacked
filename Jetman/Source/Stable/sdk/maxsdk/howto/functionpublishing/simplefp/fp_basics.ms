fileIn "MxsUnit/MxsUnit.ms"

struct fpbasics_UnitTest
(
	function Test_Number = 
	(
		fpbasics.number = 5.6
		assert_true(5.6 == fpbasics.number)
		
		local result = fpbasics.products 2.0 7.0
		assert_true( 14.0 == result )
	),
	-- by value
	function Test_Position = 
	(
		local default_pos = fpbasics.position
		assert_true( 0 == default_pos.x)
		assert_true( 0 == default_pos.y)
		
		local pos = fpbasics.position = point2 34 56
		assert_true( 34.0 == pos.x)
		assert_true( 56.0 == pos.y)
	),
	function foo = 
	(
		local pos = point2 11 22
		fpbasics.position = pos
		local setVal = fpbasics.position
		assert_true( 11.0 == setVal.x ) 
		assert_true( 22.0 == setVal.y ) 
	),
	function Test_Position_Scope = 
	(
		fpbasics.position = point2 35 36
		local val = fpbasics.position
		assert_true( 35.0 == val.x ) 
		assert_true( 35.0 == val.y ) 
		foo()
		-- AT this point, pos falls off the stack and should get cleaned up.
		
		-- Assert that the position property is still set from foo.
		local res = fpbasics.position
		assert_true( 11.0 == res.x ) 
		assert_true( 22.0 == res.y ) 
	),
	-- by reference
	function Test_Location = 
	(
		local default_val = fpbasics.location
		assert_true( 0 == default_val.x)
		assert_true( 0 == default_val.y)
		
		local val = fpbasics.location = point2 67 89
		assert_true( 67.0 == val.x)
		assert_true( 89.0 == val.y)
	),
	function baz = 
	(
		local loc = point2 77 67
		fpbasics.location = loc
		local setVal = fpbasics.location
		assert_true( 77.0 == setVal.x )
		assert_true( 67.0 == setVal.y )
	),
	function Test_Location_Scope = 
	(
		fpbasics.location = point2 78 96
		local val = fpbasics.location
		assert_true( 78.0 == val.x ) 
		assert_true( 96.0 == val.y ) 
		baz()
		-- At this point, loc falls off the stack and should get cleaned up.
		
		-- Assert that the location property is still set from baz.
		local res = fpbasics.location
		assert_true( 77.0 == res.x )
		assert_true( 67.0 == res.y )
	),
	Tests = 
	#(
		Test_Number,
		Test_Position,
		Test_Position_Scope,
		Test_Location,
		Test_Location_Scope
	)
)
run_test_fixture fpbasics_UnitTest script:(getThisScriptFilename())