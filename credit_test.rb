require_relative "credit.rb"
require "minitest/autorun"

class Test1 < MiniTest::Test
	def setup
		@Susan = Credit.new(1000, 0.35)
	end

	def test_the_initlization
		assert_equal 1000, @Susan.balance
		assert_equal 0.35, @Susan.APR
	end

	def test_withdraw
		@Susan.withdraw(500)
		assert_equal 500, @Susan.balance
	end

	def test_deposit
		@Susan.deposit(300)
		assert_equal 1300, @Susan.balance
	end

	def test_interest
		@Susan.withdraw(500)
		assert_equal 14.38, @Susan.calculateInterest
	end
end

