package main;

import players.*;
import institutions.*;
import java.util.ArrayList;
import java.util.Random;

public class Main {

    public static String generatePersonalId() {
        StringBuilder sb = new StringBuilder();
        Random rand = new Random();
        for (int i = 0; i < 11; i++) {
            sb.append(rand.nextInt(10));
        }
        return sb.toString();
    }

    public static String generateFirstName() {
        String[] names = {"John", "Carol", "Mary", "Sophie", "Thomas", "Albert", "Gregory", "Ann",
                "Caroline", "Francis", "Theo", "Patricia", "Barbara", "Lucy"};
        return names[new Random().nextInt(names.length)];
    }

    public static String generateLastName() {
        String[] names = {"Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller", "Davis",
                "Rodriguez", "Martinez", "Hernandez", "Lopez", "Gonzalez"};
        return names[new Random().nextInt(names.length)];
    }

    public static void runSimulation() {
        StateTreasury treasury = StateTreasury.getInstance();
        treasury.reset();

        Headquarters headquarters = new Headquarters(treasury);

        for (int i = 0; i < 10; i++) {
            headquarters.addOutlet();
        }

        ArrayList<Player> players = new ArrayList<>();
        Random rand = new Random();

        // 1. Random Players
        for (int i = 0; i < 200; i++) {
            players.add(new RandomPlayer(generateFirstName(), generateLastName(), generatePersonalId(), headquarters));
        }

        // 2. Minimalist Players
        for (int i = 0; i < 200; i++) {
            int outletIndex = i % 10;
            LotteryOutlet outlet = headquarters.getOutlets().get(outletIndex);
            players.add(new MinimalistPlayer(generateFirstName(), generateLastName(), generatePersonalId(), outlet));
        }

        // 3. Constant Ticket Players
        for (int i = 0; i < 200; i++) {
            TicketForm form = new TicketForm();
            for (int k = 1; k <= 8; k++) {
                if (rand.nextBoolean()) {
                    for(int n=0; n<6; n++) form.markNumber(k, rand.nextInt(49)+1);
                }
            }
            if (form.countValidBets() == 0) {
                for(int n=0; n<6; n++) form.markNumber(1, rand.nextInt(49)+1);
            }
            form.markNumberOfDraws(5);

            ArrayList<LotteryOutlet> favOutlets = new ArrayList<>();
            for (int j = 0; j < 5; j++) {
                favOutlets.add(headquarters.getOutlets().get((i + j) % 10));
            }

            players.add(new ConstantTicketPlayer(generateFirstName(), generateLastName(), generatePersonalId(),
                    form, rand.nextInt(10), favOutlets, headquarters));
        }

        // 4. Constant Number Players
        for (int i = 0; i < 200; i++) {
            int[] nums = new int[6];
            for (int j = 0; j < 6; j++) nums[j] = rand.nextInt(49) + 1;

            ArrayList<LotteryOutlet> favOutlets = new ArrayList<>();
            for (int j = 0; j < 5; j++) {
                favOutlets.add(headquarters.getOutlets().get((i + j) % 10));
            }

            players.add(new ConstantNumbersPlayer(generateFirstName(), generateLastName(), generatePersonalId(),
                    nums, rand.nextInt(10), favOutlets, headquarters));
        }

        // Simulation Loop
        for (int i = 0; i < 20; i++) {
            for (Player p : players) p.buy();
            headquarters.conductDraw();
            for (Player p : players) p.checkResults();
        }

        headquarters.printHistory();
        System.out.println(treasury);
    }

    public static void main(String[] args) {
        runSimulation();
    }
}