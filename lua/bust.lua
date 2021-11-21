describe(
    "Busted unit testing framework",
    function()
        describe(
            "should be awesome",
            function()
                it(
                    "should be easy to use",
                    function()
                        assert.truthy("Yup.")
                    end
                )
                it(
                    "should have lots of features",
                    function()
                        -- deep check comparisons!
                        assert.are.same({table = "great"}, {table = "great"})

                        -- or check by reference!
                        assert.are_not.equal({table = "great"}, {table = "great"})

                        assert.truthy("this is a string") -- truthy: not false or nil

                        assert.True(1 == 1)
                        assert.is_true(1 == 1)

                        assert.falsy(nil)
                        assert.has_error(
                            function()
                                error("Wat")
                            end,
                            "Wat"
                        )
                    end
                )
                it(
                    "should provide some shortcuts to common functions",
                    function()
                        assert.are.unique({{thing = 1}, {thing = 2}, {thing = 3}})
                    end
                )
            end
        )

        describe(
            "spies",
            function()
                it(
                    "registers a new spy as a callback",
                    function()
                        local s =
                            spy.new(
                            function()
                            end
                        )

                        s(1, 2, 3)
                        s(4, 5, 6)

                        assert.spy(s).was.called()
                        assert.spy(s).was.called(2) -- twice!
                        assert.spy(s).was.called_with(1, 2, 3) -- checks the history
                    end
                )

                it(
                    "replaces an original function",
                    function()
                        local t = {
                            greet = function(msg)
                                print(msg)
                            end
                        }

                        local s = spy.on(t, "greet")

                        t.greet("Hey!") -- prints 'Hey!'
                        assert.spy(t.greet).was_called_with("Hey!")

                        t.greet:clear() -- clears the call history
                        assert.spy(s).was_not_called_with("Hey!")

                        t.greet:revert() -- reverts the stub
                        t.greet("Hello!") -- prints 'Hello!', will not pass through the spy
                        assert.spy(s).was_not_called_with("Hello!")
                    end
                )
            end
        )

        describe(
            "stubs",
            function()
                it(
                    "replaces an original function",
                    function()
                        local t = {
                            greet = function(msg)
                                print(msg)
                            end
                        }

                        stub(t, "greet")

                        t.greet("Hey!") -- DOES NOT print 'Hey!'
                        assert.stub(t.greet).was.called_with("Hey!")

                        t.greet:revert() -- reverts the stub
                        t.greet("Hey!") -- DOES print 'Hey!'
                    end
                )
            end
        )

        describe(
            "mocks",
            function()
                it(
                    "replaces a table with spies",
                    function()
                        local t = {
                            thing = function(msg)
                                print(msg)
                            end
                        }

                        local m = mock(t) -- mocks the table with spies, so it will print

                        m.thing("Coffee")
                        assert.spy(m.thing).was.called_with("Coffee")
                    end
                )

                it(
                    "replaces a table with stubs",
                    function()
                        local t = {
                            thing = function(msg)
                                print(msg)
                            end
                        }

                        local m = mock(t, true) -- mocks the table with stubs, so it will not print

                        m.thing("Coffee")
                        assert.stub(m.thing).was.called_with("Coffee")
                        mock.revert(m) -- reverts all stubs/spies in m
                        m.thing("Tea") -- DOES print 'Tea'
                    end
                )
            end
        )

        describe(
            "match arguments",
            function()
                local match = require("luassert.match")

                it(
                    "tests wildcard matcher",
                    function()
                        local s =
                            spy.new(
                            function()
                            end
                        )
                        local _ = match._

                        s("foo")

                        assert.spy(s).was_called_with(_) -- matches any argument
                        assert.spy(s).was_not_called_with(_, _) -- does not match two arguments
                    end
                )

                it(
                    "tests type matchers",
                    function()
                        local s =
                            spy.new(
                            function()
                            end
                        )

                        s("foo")

                        assert.spy(s).was_called_with(match.is_string())
                        assert.spy(s).was_called_with(match.is_truthy())
                        assert.spy(s).was_called_with(match.is_not_nil())
                        assert.spy(s).was_called_with(match.is_not_false())
                        assert.spy(s).was_called_with(match.is_not_number())
                        assert.spy(s).was_called_with(match.is_not_table())
                    end
                )

                it(
                    "tests more matchers",
                    function()
                        local s =
                            spy.new(
                            function()
                            end
                        )

                        s(1)

                        assert.spy(s).was_called_with(match.is_equal(1))
                        assert.spy(s).was_called_with(match.is_same(1))
                    end
                )
            end
        )

        describe(
            "combine matchers",
            function()
                local match = require("luassert.match")

                it(
                    "tests ref matchers for passed in table",
                    function()
                        local t = {cnt = 0}
                        function t.incrby(t, i)
                            t.cnt = t.cnt + i
                        end

                        local s = spy.on(t, "incrby")

                        s(t, 2)

                        assert.spy(s).was_called_with(match.is_ref(t), 2)
                    end
                )

                it(
                    "tests ref matchers for self",
                    function()
                        local t = {cnt = 0}
                        function t:incrby(i)
                            self.cnt = self.cnt + i
                        end

                        local s = spy.on(t, "incrby")

                        t:incrby(2)

                        assert.spy(s).was_called_with(match.is_ref(t), 2)
                    end
                )
            end
        )
    end
)
