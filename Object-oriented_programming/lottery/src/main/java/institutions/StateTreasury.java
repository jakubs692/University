package institutions;

/**
 * Represents the State Treasury (Budget).
 * It collects taxes from lottery winnings and provides subsidies when needed.
 * Implements the Singleton pattern.
 */
public class StateTreasury {
    private static final StateTreasury INSTANCE = new StateTreasury();
    private long collectedTaxes;
    private long providedSubsidies;

    private StateTreasury() { }

    public static StateTreasury getInstance() { return INSTANCE; }

    /**
     * Resets the treasury state. Useful for running fresh simulations.
     */
    public void reset() {
        this.collectedTaxes = 0;
        this.providedSubsidies = 0;
    }

    protected long grantSubsidy(long amount){
        this.providedSubsidies += amount;
        return amount;
    }

    protected long collectTaxes(long amount){
        this.collectedTaxes += amount;
        return amount;
    }

    public long getCollectedTaxes() {
        return collectedTaxes;
    }

    public long getProvidedSubsidies() {
        return providedSubsidies;
    }

    @Override
    public String toString() {
        return String.format("State Treasury: Collected Taxes: %.2f PLN, Provided Subsidies: %.2f PLN",
                collectedTaxes / 100.0, providedSubsidies / 100.0);
    }
}