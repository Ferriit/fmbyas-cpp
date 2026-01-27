import java.io.*;
import javax.swing.*;
import java.awt.*;
import java.nio.file.*;
import java.util.List;
import java.util.stream.Collectors;

public class App {
    public static void main(String[] args) {
        JFrame frame = new JFrame("Assembler");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(800, 600);

        JComboBox<String> fileList = new JComboBox<>();
        JTextArea codeArea = new JTextArea(20, 60);
        JButton assembleButton = new JButton("Assemble");
        JTextArea outputArea = new JTextArea(8, 60);
        outputArea.setEditable(false);

        String os = System.getProperty("os.name").toLowerCase();
        boolean isWindows = os.contains("win");
        String assemblerBinary = isWindows ? "../dist/assembler.exe" : "../dist/assembler";
        Path assemblerPath = Paths.get(assemblerBinary);

        // find the fucking files bro
try {
    Path testDir = Paths.get("..", "tests");
    if (Files.exists(testDir) && Files.isDirectory(testDir)) {
        List<String> files = Files.list(testDir)
                .filter(p -> p.toString().endsWith(".s"))
                .map(p -> p.getFileName().toString())
                .collect(Collectors.toList());

        for (String f : files) {
            fileList.addItem(f);
        }

        if (!files.isEmpty()) {
            String firstFile = files.get(0);
            fileList.setSelectedItem(firstFile);

            Path filePath = testDir.resolve(firstFile);
            try {
                String content = new String(Files.readAllBytes(filePath));
                codeArea.setText(content);
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
    }
} catch (IOException e) {
    e.printStackTrace();
}
        fileList.addActionListener(e -> {
            String selected = (String) fileList.getSelectedItem();
            if (selected != null) {
                Path filePath = Paths.get("..", "tests", selected);
                try {
                    String content = new String(Files.readAllBytes(filePath));
                    codeArea.setText(content);
                } catch (IOException ex) {
                    ex.printStackTrace();
                }
            }
        });

        assembleButton.addActionListener(e -> {
            String selected = (String) fileList.getSelectedItem();
            if (selected == null) {
                JOptionPane.showMessageDialog(frame, "Select a file first!");
                return;
            }

            Path filePath = Paths.get("..", "tests", selected);

            try {
                if (!Files.exists(assemblerPath)) {
                    JOptionPane.showMessageDialog(frame, "Assembler binary not found. Compiling...");
                    String cppSource = isWindows ? "../src/assembler.cpp" : "../src/assembler.cpp";
                    ProcessBuilder compile = new ProcessBuilder(
                            "g++", "-std=c++17", cppSource, "-o", assemblerBinary
                    );
                    compile.redirectErrorStream(true);
                    Process compileProcess = compile.start();

                    BufferedReader compileReader = new BufferedReader(
                            new InputStreamReader(compileProcess.getInputStream())
                    );
                    StringBuilder compileOutput = new StringBuilder();
                    String line;
                    while ((line = compileReader.readLine()) != null) {
                        compileOutput.append(line).append("\n");
                    }
                    compileProcess.waitFor();

                    outputArea.setText("Compilation output:\n" + compileOutput.toString());
                }

                ProcessBuilder pb = new ProcessBuilder(assemblerBinary, filePath.toString());
                pb.redirectErrorStream(true);
                Process process = pb.start();

                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                outputArea.setText("");
                String line;
                while ((line = reader.readLine()) != null) {
                    outputArea.append(line + "\n");
                }

                process.waitFor();

            } catch (IOException | InterruptedException ex) {
                ex.printStackTrace();
            }
        });

        JPanel topPanel = new JPanel();
        topPanel.add(new JLabel("Select File:"));
        topPanel.add(fileList);
        topPanel.add(assembleButton);

        frame.setLayout(new BorderLayout());
        frame.add(topPanel, BorderLayout.NORTH);
        frame.add(new JScrollPane(codeArea), BorderLayout.CENTER);
        frame.add(new JScrollPane(outputArea), BorderLayout.SOUTH);

        frame.setVisible(true);
    }
}
