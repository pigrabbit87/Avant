require 'date'

class Credit
	def initialize(credit_limit, rate)
		@credit_limit = credit_limit
		@APR = rate
		@balance = credit_limit
		@started_date = Date.today
		@transactions = []
	end

	def withdraw(amount)
		if amount > @balance
			puts "You are trying to withdraw more than your remaining credit"
		else
			remaining_periods = (Date.today - @started_date + 1).to_i % 30
			@balance -= amount
			puts "You just withdrew $#{amount}. You have $#{@balance} left in your account"
			current = [remaining_periods, -1 * amount]
			@transactions << current
		end
	end

	def deposit(amount)
		remaining_periods = (Date.today - @started_date + 1).to_i % 30
		@balance += amount
		puts "You just pay $#{amount}. You now have $#{@balance} in your account"
		current = [remaining_periods, amount]
		@transactions << current
	end

	# Calculate the interest at the end of the the month
	# We can calculate the interest by using the transaction log
	# Interest is calculate by summing the interest acculated till the end of the 30 days
	# For example, if we withdraw $100 at day one, the interest will be 29 * $100 * APR / 365 at day 30
	# If $100 is paid back at day 2, the interest will be (30 * $100 * APR / 365) - (29 * $100 * APR / 365)
	def calculateInterest
		interest = 0
		@transactions.each do |trans|
			interest += -1 * ((31 - trans[0]) * trans[-1] * @APR / 365)
		end
		interest < 0 ? 0 : interest.round(2)
	end

	def checklog
		@transactions.each do |log|
			puts "Day " + log[0].to_s + " - " + (log[1] > 0 ? "Deposit $" : "Withdraw $") + log[1].abs.to_s
		end
	end

	def checkPayment
		if (Date.today - @started_date + 1).to_i % 30 != 0 
			puts "Your payment is not due for another " + (30 - (Date.today - @started_date + 1).to_i % 30).to_s + " days"
			puts "You currently owe " + (@credit_limit - @balance).to_s 
		end
		puts "You have $" + (calculateInterest + @credit_limit - @balance).to_s + " due at this time"
	end
end
